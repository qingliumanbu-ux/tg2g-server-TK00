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
BM2F_ENTERACE(tksm01_inq2)
//-EP_SYSTEM_HEAD_END

int f_tksm01_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString		mat_type = " ";

	CModel ttksm02("TTKSM02");


	CString  sqlstr("");
	CDecimal prod_wt = 0;


	CDbCommand cmd_inq(conn);

	try
	{
		/*begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
*/
		ttksm02.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//ttksm02["HEAT_NO"] = "B1307889";
		sqlstr = " select sum(prod_wt)"
			" from ttksm01"
			" where heat_no=@heat_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", ttksm02["HEAT_NO"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			prod_wt = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();

		sqlstr = "select t.heat_no,t.mat_code,t.mat_name,t.equ_no,t.CO2_WT,t.wt,t.CO2_COE,t.CO2_COE_UNIT,t.HOT_VAL,t.HOT_VAL_UNIT,t.HANDLE_DIV,t.cost_center,t.mat_code_t,t2.type_code1,t2.type_code,t2.unit"
			", decode("+prod_wt.ToString()+", 0, 0, round(CO2_WT / "+prod_wt.ToString()+",3)) CO2_WT_UNIT"
			",PRICE,COST"
			" from ttksm02 t"
			" left join ttk0001 t2 on t.mat_code=t2.mat_code"
			" where t.heat_no=@heat_no"
			" order by t2.type_code1,t.mat_code desc"
			;
		Log::Trace("", __FUNCTION__, "heat_no = [{0}]", ttksm02["HEAT_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", ttksm02["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("prod_wt", prod_wt);
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