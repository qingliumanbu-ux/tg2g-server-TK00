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
BM2F_ENTERACE(tk0007_inq2)
//-EP_SYSTEM_HEAD_END
int f_tk0007_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
			" from ttk0006"
			" where 1=1"
			" and TYPE_DESC = '主原料'"
			" and st_no = @st_no  "
			" union all"
			" select *"
			" from ttk0006"
			" where 1=1"
			" and TYPE_DESC = '钢种能源'"
			" and st_no = @st_no  "
			" union all"
			" select *"
			" from ttk0006"
			" where 1=1"
			" and @whole_backlog like  '%'||SUB_BACKLOG_CODE||'%'"
			" and TYPE_DESC = '工序费'" 
			" union all"
			" select *"		//连铸
			" from ttk0006"
			" where 1=1"
			" and SUB_BACKLOG_CODE = @sub_backlog_code"
			" and TYPE_DESC = '工序费'"  			
			;		

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", ttk0005["ST_NO"].ToString());
		cmd_inq.Parameters.Set("whole_backlog", ttk0005["WHOLE_BACKLOG"].ToString());
		if (ttk0005["ST_NO"].ToString().SubstringNE(0, 1) == "1" || ttk0005["ST_NO"].ToString().SubstringNE(0, 1) == "4")
		{
			cmd_inq.Parameters.Set("sub_backlog_code", "1C");
		}
		else
		{
			cmd_inq.Parameters.Set("sub_backlog_code", "2C");
		}		
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