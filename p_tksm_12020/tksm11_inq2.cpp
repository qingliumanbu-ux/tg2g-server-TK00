/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:碳控排组成
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm11_inq2)
//-EP_SYSTEM_HEAD_END

int f_tksm11_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString st_no("");

	CString backlog_ea("");

	CModel ttksm02("TTKSM02");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);

	try
	{
		begin_time = bcls_rec->Tables["PARA"].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables["PARA"].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();  
		backlog_ea = bcls_rec->Tables[0].Rows[0]["BACKLOG_EA"].ToString();

		sqlstr = " select t1.mat_code,wt,t1.co2_wt,t1.co2_wt1,t1.co2_wt2,prod_wt"
			" ,nvl(t2.mat_name,' ') mat_name,nvl(t2.type_code1,' ') type_code1"
			" ,decode(nvl(prod_wt,0),0,0,round(t1.co2_wt/prod_wt,4)) co2_wt_unit"
			" ,decode(nvl(prod_wt,0),0,0,round(t1.co2_wt1/prod_wt,4)) co2_wt_unit1"
			" ,decode(nvl(prod_wt,0),0,0,round(t1.co2_wt2/prod_wt,4)) co2_wt_unit2"
			" ,decode(nvl(prod_wt,0),0,0,round(wt/prod_wt,6)) wt_unit"
			" ,decode(nvl(wt,0),0,0,round(t1.co2_wt/wt,6)) CO2_coe"
			" ,decode(nvl(wt,0),0,0,round(t1.co2_wt1/wt,6)) CO2_coe1"
			" ,decode(nvl(wt,0),0,0,round(t1.co2_wt2/wt,6)) CO2_coe2"
			" from "
			"("
			" select mat_code,sum(wt) wt,sum(co2_wt) co2_wt,sum(co2_wt1) co2_wt1,sum(co2_wt2) co2_wt2"
			" ,nvl((select sum(prod_wt) prod_wt from ttksm01 where 1=1  and backlog_ea=@backlog_ea and st_no=@st_no  and prod_time <= @end_time  and prod_time>=@begin_time),0) prod_wt"	 
			" from ttksm02"
			" where 1=1"
			" and heat_no in ( select heat_no from ttksm01 where 1=1"
			" and backlog_ea=@backlog_ea"
			" and st_no=@st_no"
			" and prod_time <= @end_time"
			" and prod_time>=@begin_time"
			" )"
			" group by mat_code"
			" ) t1 left join ttk0001 t2 on t1.mat_code =t2.mat_code"
			" order by nvl(t2.type_code1,' '),nvl(t2.SEQ_NO,0),t1.mat_code "
			;  
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("st_no", st_no);
		cmd_inq.Parameters.Set("backlog_ea", backlog_ea);
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