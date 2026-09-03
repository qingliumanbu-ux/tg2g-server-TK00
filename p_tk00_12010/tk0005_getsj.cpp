/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:碳排因子查询
**************************************************/
//框架用头文件
#include "stdafx.h"
#include "epex.h"  //使用同步路由头文件
// service入口
BM2F_ENTERACE(tk0005_getsj)
//-EP_SYSTEM_HEAD_END
int f_tk00_getsm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk0005_getsj(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString account_period("");

	CString		mat_type = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel ttk0004("TTK0004");
	CModel ttksm01("TTKSM01");
	CModel ttksm02("TTKSM02");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("ACCOUNT_PERIOD"))
		{
			account_period = bcls_rec->Tables[0].Rows[0]["ACCOUNT_PERIOD"].ToString().SubstringNE(0, 6);
		}
		else
		{
			account_period = CDateTime::Now().ToString("yyyyMM");
		}

	
		EIClass inBlock_tk, outBlock_tk;
		inBlock_tk.Tables[0].Columns.Add(DT_STRING, "STAT_DATE");
		inBlock_tk.Tables[0].Rows.Add();
		inBlock_tk.Tables[0].Rows[0]["STAT_DATE"] = account_period;

		doFlag = f_tk00_getsm(&inBlock_tk, &outBlock_tk, conn);
		if (doFlag != 0)
		{
			s.flag = -1;
			return -1;
		}

		sqlstr = " update ttksm02 t1 set mat_name = (SELECT mat_name FROM TTK0001 t2 where t1.mat_code=t2.mat_code)"
		" where exists (SELECT 1 FROM TTK0001 t2 where t1.mat_code=t2.mat_code)"
		" and STAT_DATE=@account_period"
		;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("account_period", account_period);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 		

		//更新  ttksm01的碳排量
		sqlstr = " update ttksm01 t1 set CO2_WT = (select sum(CO2_WT) from ttksm02 t2 where t1.heat_no=t2.heat_no)"
			" where 1=1"
			" and exists (SELECT 1 FROM ttksm02 t2 where  t1.heat_no=t2.heat_no)"
			" and STAT_DATE=@account_period"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("account_period", account_period);
		cmd_inq.ExecuteNonQuery();
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