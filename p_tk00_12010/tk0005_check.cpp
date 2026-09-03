/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:检测碳排因子信息
**************************************************/
//框架用头文件
#include "stdafx.h"
#include "epex.h"  //使用同步路由头文件
// service入口
BM2F_ENTERACE(tk0005_check)
//-EP_SYSTEM_HEAD_END
int f_tk00_getco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk00_bzco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk0005_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString account_period("");

	
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	
	CModel ttksm03("TTKSM03");
	CModel ttk0004("TTK0004");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{
		ttksm03.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		account_period = bcls_rec->Tables[0].Rows[0]["ACCOUNT_PERIOD"].ToString().SubstringNE(0, 6);

		sqlstr = " select distinct mat_code_t,mat_name_t"
			" from (" 			
			" select distinct mat_code_t,mat_name_t "
			" from ttksm02 t1"
			" where 1=1"
			" and not exists (select 1 from ttk0004 t2 where t2.mat_code_t =t1.mat_code_t)"
			" and prod_time <= @account_period||'31235959'"
			" and prod_time >= @account_period||'01'"
			" )"
			;
		Log::Trace("", "", "sqlstr={0} ACCOUNT_PERIOD={1}", sqlstr, ttksm03["ACCOUNT_PERIOD"].ToString());
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("account_period", ttksm03["ACCOUNT_PERIOD"].ToString().SubstringNE(0, 6));
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		//更新ttksm02的排放因子
		EIClass inBlock_yz, outBlock_yz;
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "DATA_TYPE");
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "VALID_TIME");
		inBlock_yz.Tables[0].Rows.Add();
		sqlstr = " select distinct mat_code from ttksm02"
			" where 1=1"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", account_period);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			inBlock_yz.Tables[0].Rows[0]["MAT_CODE"] = cmd_inq.GetString(1);
			doFlag = f_tk00_getco2(&inBlock_yz, &outBlock_yz, conn);
			if (doFlag != 0)
			{
				s.flag = -1;
				return -1;
			}
			if (outBlock_yz.Tables[0].Rows.get_Count() > 0)
			{
				Log::Trace("", "", "mat_code={0} ", cmd_inq.GetString(1));
				ttk0004.MergeFrom(outBlock_yz.Tables[0].Rows[0]);
				sqlstr = " update ttksm02 set CO2_COE= @co2_coe"
					",CO2_WT = round(wt*@co2_coe,6)"
					" ,CO2_COE_UNIT = @co2_coe_unit"
					",CO2_COE1= @co2_coe1"
					",CO2_WT1 = round(wt*@co2_coe1,6)"
					",CO2_COE2= @co2_coe2"
					",CO2_WT2 = round(wt*@co2_coe2,6)"
					" where 1=1"
					" and mat_code = @mat_code"
					" and stat_date=@stat_date"
					;
				cmd_inq_s.SetCommandText(sqlstr);
				cmd_inq_s.Parameters.Set("stat_date", account_period);
				cmd_inq_s.Parameters.Set("mat_code", cmd_inq.GetString(1));
				cmd_inq_s.Parameters.Set("mat_name", ttk0004["MAT_NAME"].ToString());
				cmd_inq_s.Parameters.Set("co2_coe", ttk0004["CO2_COE"].ToDecimal());
				cmd_inq_s.Parameters.Set("co2_coe_unit", ttk0004["CO2_COE_UNIT"].ToString());
				cmd_inq_s.Parameters.Set("co2_coe1", ttk0004["CO2_COE1"].ToDecimal());
				cmd_inq_s.Parameters.Set("co2_coe2", ttk0004["CO2_COE2"].ToDecimal());
				cmd_inq_s.ExecuteNonQuery();
				cmd_inq_s.Close();
			}
		}
		cmd_inq.Close();  


		//更新  ttksm01的碳排量
		sqlstr = " update ttksm01 t1 set (CO2_WT,CO2_WT1,CO2_WT2) = (select SUM(CO2_WT),SUM(CO2_WT1),SUM(CO2_WT2) from ttksm02 t2 where t1.heat_no=t2.heat_no)"
			" where 1=1"
			" and exists (SELECT 1 FROM ttksm02 t2 where  t1.heat_no=t2.heat_no)"
			" and STAT_DATE=@stat_date"
			;
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", account_period);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		doFlag = f_tk00_bzco2(&inBlock_yz, &outBlock_yz, conn);
		if (doFlag != 0)
		{
			s.flag = -1;
			return -1;
		}


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