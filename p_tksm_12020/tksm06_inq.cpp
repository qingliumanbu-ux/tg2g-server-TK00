/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:碳税标准
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm06_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm06_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString equ_no("");

	CString		mat_type = " ";

	CModel ttksm02("TTKSM02");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		equ_no = bcls_rec->Tables[0].Rows[0]["EQU_NO"].ToString();

		ttksm02.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "c_div = [{0}]", ttksm02["C_DIV"].ToString());

		sqlstr = 
			" select t1.prod_date,t2.prod_wt"
			",decode(t2.prod_wt,0,0,round(TOTAL_AMT/prod_wt,3) ) TOTAL_AMT"			
			" from "
			" (select substr(prod_time,1,8) prod_date ,sum(CO2_WT) TOTAL_AMT"
			" from ttksm02"
			" where prod_time<=@end_time"
			" and prod_time>=@begin_time"
			;
		if (equ_no.Trim() != "")
			sqlstr = sqlstr + " and equ_no =@equ_no	" ;
		if (ttksm02["C_DIV"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and C_DIV=@c_div";
		}
		if (ttksm02["ST_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and ST_NO=@st_no";
		}
		sqlstr = sqlstr + " group by substr(prod_time,1,8)) t1 ";
		
		sqlstr = sqlstr + "  left join (select substr(prod_time,1,8) prod_date ,sum(prod_wt) prod_wt"
			" from ttksm01"
			" where prod_time<=@end_time"
			" and prod_time>=@begin_time"
			;
		if (ttksm02["C_DIV"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and C_DIV=@c_div";
		}
		if (ttksm02["ST_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and ST_NO=@st_no";
		}
		sqlstr = sqlstr + " group by substr(prod_time,1,8)) t2 "
			" on t1.prod_date =t2.prod_date"
			" order by t1.prod_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("equ_no", equ_no);
		cmd_inq.Parameters.Set("c_div", ttksm02["C_DIV"].ToString());
		cmd_inq.Parameters.Set("st_no", ttksm02["ST_NO"].ToString());
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