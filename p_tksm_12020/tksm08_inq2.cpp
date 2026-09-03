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
BM2F_ENTERACE(tksm08_inq2)
//-EP_SYSTEM_HEAD_END

int f_tksm08_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString st_no("");
	CString ac_route("");

	CString		mat_type = " ";

	//CModel tcaais5("TCAAIS5");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{
		st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);   		

		Log::Trace("", __FUNCTION__, "begin_time = [{0}],end_time = [{1}],st_no = [{2}]", begin_time, end_time, st_no);

		sqlstr = " select ac_route,sum(prod_wt) prod_wt,sum(CO2_WT) c_wt,sum(AMOUNT_TAX) c_tax,decode(sum(prod_wt),0,0,cast(1.000*sum(WT_C)/sum(prod_wt) as decimal(16, 3)) ) c_wt_unit,decode(sum(prod_wt),0,0, cast(1.000*sum(AMOUNT_TAX)/sum(prod_wt) as decimal(16, 3))) c_tax_unit"
			" from ttksm01"
			" where prod_time<=@end_time"
			" and prod_time>=@begin_time"
			" and st_no = @st_no"
			" group by ac_route"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("st_no", st_no);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "条数 = [{0}]", bcls_ret->Tables[0].Rows.get_Count());


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