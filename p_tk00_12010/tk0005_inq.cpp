/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:月成本总耗用
**************************************************/
//框架用头文件
#include "stdafx.h"
#include "epex.h"  //使用同步路由头文件
// service入口
BM2F_ENTERACE(tk0005_inq)
//-EP_SYSTEM_HEAD_END
int f_tk00_getco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk0005_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString account_period("");

	
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	
	CModel ttksm03("TTKSM03");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{
		ttksm03.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		ttksm03["ACCOUNT_PERIOD"] = ttksm03["ACCOUNT_PERIOD"].ToString().SubstringNE(0,6);
		sqlstr = "select ACCOUNT_PERIOD,COST_CENTER,MAT_CODE_T,MAT_NAME_T,AC_WT "
			" ,nvl((select COST_CENTER_NAME from ttk0001c where cost_center = t.cost_center),' ') COST_CENTER_NAME"
			" from ttksm03 t"
			" where 1=1"
			" and cost_center in (select cost_center from ttk0001c where FACTORY_ID = 'LG4')"
			" and account_period = @account_period"
			;
		if (ttksm03["MAT_CODE_T"].ToString().Trim() != "")
			sqlstr = sqlstr + " and mat_code_t like @mat_code_t||'%'";
		if (ttksm03["MAT_NAME_T"].ToString().Trim() != "")
			sqlstr = sqlstr + " and mat_name_t like '%'||@mat_name_t||'%'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("account_period", ttksm03["ACCOUNT_PERIOD"].ToString());
		cmd_inq.Parameters.Set("mat_code_t", ttksm03["MAT_CODE_T"].ToString());
		cmd_inq.Parameters.Set("mat_name_t", ttksm03["MAT_NAME_T"].ToString());
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