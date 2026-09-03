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
BM2F_ENTERACE(tksm03_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm03_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString		mat_type = " ";

	CModel ttksm02("TTKSM02");


	CString  sqlstr("");
	CString  sqlstr_sub("");


	CDbCommand cmd_inq(conn);

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		ttksm02.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//取消耗项，然后将消耗项设置为横向
		sqlstr = " select distinct mat_code from ttksm02"
			" where 1=1"
			" and PROD_TIME<=@end_time"
			" and PROD_TIME>=@begin_time"
			;
		if (ttksm02["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		if (ttksm02["C_DIV"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and C_DIV=@c_div";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("heat_no", ttksm02["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("c_div", ttksm02["C_DIV"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			sqlstr_sub = sqlstr_sub + ",sum(case when mat_code ='" + cmd_inq.GetString(1) + "' then CO2_WT else 0 end) as use_" + cmd_inq.GetString(1);
			sqlstr_sub = sqlstr_sub + ",sum(case when mat_code ='" + cmd_inq.GetString(1) + "' then WT else 0 end) as use2_" + cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		sqlstr = " select t1.heat_no,t1.st_no,t1.prod_time,t1.PROD_WT,t2.* "
			",decode(PROD_WT,0,0,round(TYPE_2/prod_wt,6)) TYPE_2_UNIT"
			" from ttksm01 t1"
			" left join ("
			" select heat_no,sum(CO2_WT) as CO2_WT_all " + sqlstr_sub +
			", sum(case when  mat_code in(select mat_code from ttk0001 where type_code1 = '2') then CO2_WT else 0 end) TYPE_2"
			" from ttksm02"
			" where 1=1"
			" and PROD_TIME<=@end_time"
			" and PROD_TIME>=@begin_time"
			" group by heat_no"
			") t2 on t1.heat_no=t2.heat_no"
			" where 1=1"
			" and t1.PROD_TIME<=@end_time"
			" and t1.PROD_TIME>=@begin_time"
			;
		if (ttksm02["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and t1.heat_no=@heat_no";
		}
		if (ttksm02["C_DIV"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and C_DIV=@c_div";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("heat_no", ttksm02["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("c_div", ttksm02["C_DIV"].ToString());
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