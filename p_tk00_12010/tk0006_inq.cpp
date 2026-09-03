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
BM2F_ENTERACE(tk0006_inq)
//-EP_SYSTEM_HEAD_END
int f_tk00_bzcol(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk0006_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString		mat_type = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel ttk0006("TTK0006");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{
		ttk0006.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		sqlstr = "select *"
			" from ttk0006"
			" where 1=1"
			;
		if (ttk0006["ST_NO"].ToString().Trim() != "")
			sqlstr = sqlstr + " and st_no = @st_no";
		if (ttk0006["TYPE_DESC"].ToString().Trim() != "")
			sqlstr = sqlstr + " and type_desc = @type_desc";
		if (ttk0006["SUB_BACKLOG_CODE"].ToString().Trim() != "")
			sqlstr = sqlstr + " and sub_backlog_code = @sub_backlog_code";
		if (ttk0006["MAT_CODE"].ToString().Trim() != "")
			sqlstr = sqlstr + " and mat_code like @mat_code||'%'";
		if (ttk0006["MAT_NAME"].ToString().Trim() != "")
			sqlstr = sqlstr + " and mat_name like '%'||@mat_name||'%'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("mat_code", ttk0006["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("mat_name", ttk0006["MAT_NAME"].ToString());
		cmd_inq.Parameters.Set("type_desc", ttk0006["TYPE_DESC"].ToString());
		cmd_inq.Parameters.Set("sub_backlog_code", ttk0006["SUB_BACKLOG_CODE"].ToString());
		cmd_inq.Parameters.Set("st_no", ttk0006["ST_NO"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close(); 

		if (ttk0006["MAT_CODE"].ToString().Trim() == "col")
		{
			EIClass inBlock_yz, outBlock_yz;
			inBlock_yz.Tables[0].Columns.Add(DT_STRING, "TYPE_DESC");
			inBlock_yz.Tables[0].Rows.Add();
			doFlag = f_tk00_bzcol(&inBlock_yz, &outBlock_yz, conn);
			if (doFlag != 0)
			{
				s.flag = -1;
				return -1;
			}
		}

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