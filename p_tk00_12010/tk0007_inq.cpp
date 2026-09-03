/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:钢种工序标准碳排
**************************************************/
//框架用头文件
#include "stdafx.h"
#include "epex.h"  //使用同步路由头文件
// service入口
BM2F_ENTERACE(tk0007_inq)
//-EP_SYSTEM_HEAD_END
int f_tk0007_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString		mat_type = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel ttk0006("TTK0006");
	CModel ttk0005("TTK0005");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{
		ttk0005.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		sqlstr = "select *"
			" from ttk0005"
			" where 1=1"
			;
		if (ttk0005["C_DIV"].ToString()== "1")
			sqlstr = sqlstr + " and substr(st_no,1,1) in ('1','4')";
		if (ttk0005["C_DIV"].ToString() == "2")
			sqlstr = sqlstr + " and substr(st_no,1,1) in ('2','3','5')";
		if (ttk0005["ST_NO"].ToString().Trim() != "")
			sqlstr = sqlstr + " and st_no = @st_no";
		if (ttk0005["WHOLE_BACKLOG"].ToString().Trim() != "")
			sqlstr = sqlstr + " and WHOLE_BACKLOG like '%'||@whole_backlog||'%'"
			;  

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", ttk0005["ST_NO"].ToString());
		cmd_inq.Parameters.Set("whole_backlog", ttk0005["WHOLE_BACKLOG"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close(); 

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;
}