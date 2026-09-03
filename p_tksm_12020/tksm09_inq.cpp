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
BM2F_ENTERACE(tksm09_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm09_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString st_no("");

	CString		mat_type = " ";

	//CModel tcaais5("TCAAIS5");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();  

		sqlstr = " select a.st_no,a.backlog_ea as AC_ROUTE,a.prod_wt,a.c_wt,a.c_tax,a.c_tax_unit,c_wt_unit"
			",NVL(b.C_TS,0) C_TS,NVL(b.C_FG,0) C_FG,NVL(b.C_HJ,0) C_HJ,NVL(b.C_FL,0) C_FL,NVL(b.C_NY1,0) C_NY1,NVL(b.C_NY2,0) C_NY2"
			" from"
			" (select st_no,backlog_ea,sum(prod_wt) prod_wt,sum(CO2_WT) c_wt,sum(AMOUNT_TAX) c_tax,decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(prod_wt) ,3) ) c_wt_unit,decode(sum(prod_wt),0,0, round(sum(AMOUNT_TAX)/sum(prod_wt),3)) c_tax_unit"
			" from ttksm01"
			" where prod_time<=@end_time"
			" and prod_time>=@begin_time"		
			;
		if (st_no.Trim() != "")
			sqlstr = sqlstr + " and st_no = @st_no";
		sqlstr = sqlstr + " group by st_no,backlog_ea) a"
			" left join "
			" (select st_no,backlog_ea,sum(case when mat_code ='TS0000' then  CO2_WT else 0 end) c_ts"
			",sum(case when mat_code in (select mat_code from ttk0001 where mat_code !='TS0000' and TYPE_CODE1 = '1' ) then CO2_WT else 0 end) C_FG"
			",sum(case when mat_code in (select mat_code from ttk0001 where TYPE_CODE1 = '2' ) then CO2_WT else 0 end) C_HJ"
			",sum(case when mat_code in (select mat_code from ttk0001 where TYPE_CODE1 = '3' ) then CO2_WT else 0 end) C_FL"
			",sum(case when mat_code in (select mat_code from ttk0001 where MAT_CODE  in ('59100','59101','59102','59103')  ) then CO2_WT else 0 end) C_NY1"
			",sum(case when mat_code in (select mat_code from ttk0001 where MAT_CODE  not in ('59100','59101','59102','59103')  and  TYPE_CODE1 = '4' ) then CO2_WT else 0 end) C_NY2"
			" from ttksm02"
			" where prod_time<=@end_time"
			" and prod_time>=@begin_time"
			;
		if (st_no.Trim() != "")
			sqlstr = sqlstr + " and st_no = @st_no";
		sqlstr = sqlstr + " group by st_no,backlog_ea) b"
			" on a.st_no = b.st_no and a.backlog_ea=b.backlog_ea"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("st_no", st_no);
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