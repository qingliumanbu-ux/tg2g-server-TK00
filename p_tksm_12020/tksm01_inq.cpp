/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:查询
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm01_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm01_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString		mat_type = " ";

	CModel ttksm01("TTKSM01");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);

		ttksm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		sqlstr = "select t1.*"
			" ,(select sum(co2_wt) from ttksm02 t2 where HANDLE_DIV=' ' and  t1.heat_no=t2.heat_no)  co2_wt_sj"
			" ,(select sum(co2_wt) from ttksm02 t2 where  HANDLE_DIV='F' and  t1.heat_no=t2.heat_no)  co2_wt_ft"
			" ,(select sum(co2_wt) from ttksm02 t2 where  t1.heat_no=t2.heat_no)  co2_wt"
			" ,decode(prod_wt,0,0,(select round(sum(co2_wt)/prod_wt,6) from ttksm02 t2 where HANDLE_DIV=' ' and  t1.heat_no=t2.heat_no))  co2_wt_unit_sj"
			" ,decode(prod_wt,0,0,(select round(sum(co2_wt)/prod_wt,6) from ttksm02 t2 where  HANDLE_DIV='F' and  t1.heat_no=t2.heat_no))  co2_wt_unit_ft"
			" ,decode(prod_wt,0,0,(select round(sum(co2_wt)/prod_wt,6) from ttksm02 t2 where  t1.heat_no=t2.heat_no))  co2_wt_unit"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code = 'TS0000' and t1.heat_no=t2.heat_no)  IRON_CO2"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in (select mat_code from ttk0001 where type_code1='1') and t1.heat_no=t2.heat_no)  SCRAP_WT"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in (select mat_code from ttk0001 where type_code1='2') and t1.heat_no=t2.heat_no)  ALLOY_WT"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in (select mat_code from ttk0001 where type_code1='3') and t1.heat_no=t2.heat_no)  FL_WT"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in (select mat_code from ttk0001 where type_code1='4') and t1.heat_no=t2.heat_no)  NY_WT"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in ('59100','59101','59102','59103') and t1.heat_no=t2.heat_no)  POWER_WT"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in ('48081') and t1.heat_no=t2.heat_no)  GAS_WT"
			" from ttksm01 t1"
			" where prod_time<=@end_time"
			" and prod_time>=@begin_time"
			;
		if (ttksm01["HEAT_NO"].ToString().Trim() != "")
			sqlstr = sqlstr + " and heat_no=@heat_no";
		if (ttksm01["C_DIV"].ToString().Trim() != "")
			sqlstr = sqlstr + " and c_div=@c_div";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("heat_no", ttksm01["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("c_div", ttksm01["C_DIV"].ToString());
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