/// <summary>
/// 功能说明: 碳排模拟保存
/// </summary>
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company:   上海宝信软件股份有限公司
/// Author:    
/// Version:   1.0
/// History:
///		

#include "stdafx.h"
// Service 入口
BM2F_ENTERACE(tksm10_pf_save)
int f_tk00_getco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tksm10_pf_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	int rowCount = 0;
	CString proc_div = "";
	CString mat_code = "";
	CDecimal  seq_no_pf = 0;
	CString  sqlstr("");
	int i = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		//更新ttksm02的排放因子
		EIClass inBlock_yz, outBlock_yz;
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "DATA_TYPE");
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "VALID_TIME");
		inBlock_yz.Tables[0].Rows.Add();

		CDbCommand cmd_inq(conn);

		CModel ttksm13("TTKSM13");
		CModel ttk0004("TTK0004");

		if (bcls_rec->Tables.Contains("PARA"))
		{
			proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString();
		}
		cmd_inq.SetCommandText("select  nvl(MAX(SEQ_NO_RECIPE),0)+1 from ttksm13 ");
		seq_no_pf = cmd_inq.ExecuteScalar();
		cmd_inq.Close();

		//新增配方
		if (bcls_rec->Tables.Contains("PARA") && bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString()=="I")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				ttksm13.Reset();
				ttksm13.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				ttksm13["SEQ_NO_RECIPE"] = seq_no_pf;
				inBlock_yz.Tables[0].Rows[0]["MAT_CODE"] = ttksm13["MAT_CODE"];
				doFlag = f_tk00_getco2(&inBlock_yz, &outBlock_yz, conn);
				if (doFlag != 0)
				{
					s.flag = -1;
					return -1;
				}
				ttk0004.Reset();
				if (outBlock_yz.Tables[0].Rows.get_Count() > 0)
				{
					ttk0004.MergeFrom(outBlock_yz.Tables[0].Rows[0]);
				}
				ttksm13["CO2_COE"] = ttk0004["CO2_COE"].ToDecimal();
				ttksm13["CO2_WT"] = ttksm13["CO2_COE"] * ttksm13["DEVO_WT"].ToDecimal();
				ttksm13["REC_CREATOR"] = s.userid;
				ttksm13["REC_CREATE_TIME"] = dateNow;
				ttksm13.Insert();

			}
		}
		//配方维护-删除配方
		if (bcls_rec->Tables.Contains("PARA") && bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString() == "D")
		{
		
			Log::Trace("", "", "删除,count=[{0}]", bcls_rec->Tables[0].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				ttksm13.Reset();
				ttksm13.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				ttksm13.Delete("SEQ_NO_RECIPE");

			}
		}
		//配方明细-增删改
		if (bcls_rec->Tables.Contains("ADD_S"))
		{
			if (bcls_rec->Tables.Contains("MAIN"))
			{
				seq_no_pf = bcls_rec->Tables["MAIN"].Rows[0]["SEQ_NO_RECIPE"].ToDecimal();
			}
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD_S"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["ADD_S"].Rows.get_Count(); i++)
			{
				ttksm13.Reset();
				ttksm13.MergeFrom(bcls_rec->Tables["ADD_S"].Rows[i]);
				ttksm13["SEQ_NO_RECIPE"] = seq_no_pf;
				inBlock_yz.Tables[0].Rows[0]["MAT_CODE"] = ttksm13["MAT_CODE"];
				doFlag = f_tk00_getco2(&inBlock_yz, &outBlock_yz, conn);
				if (doFlag != 0)
				{
					s.flag = -1;
					return -1;
				}
				ttk0004.Reset();
				if (outBlock_yz.Tables[0].Rows.get_Count() > 0)
				{
					ttk0004.MergeFrom(outBlock_yz.Tables[0].Rows[0]);
				}
				ttksm13["CO2_COE"] = ttk0004["CO2_COE"].ToDecimal();
				ttksm13["CO2_WT"] = ttksm13["CO2_COE"] * ttksm13["DEVO_WT"].ToDecimal();
				ttksm13["REC_CREATOR"] = s.userid;
				ttksm13["REC_CREATE_TIME"] = dateNow;
				ttksm13.Insert();

			}


		}
		if (bcls_rec->Tables.Contains("UPD_S"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD_S"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD_S"].Rows.get_Count(); i++)
			{
				ttksm13.Reset();
				ttksm13.MergeFrom(bcls_rec->Tables["UPD_S"].Rows[i]);
				ttksm13.TrimOrBlank();

				ttksm13.Delete("SEQ_NO_RECIPE,MAT_CODE");
				
				inBlock_yz.Tables[0].Rows[0]["MAT_CODE"] = ttksm13["MAT_CODE"];
				doFlag = f_tk00_getco2(&inBlock_yz, &outBlock_yz, conn);
				if (doFlag != 0)
				{
					s.flag = -1;
					return -1;
				}
				ttk0004.Reset();
				if (outBlock_yz.Tables[0].Rows.get_Count() > 0)
				{
					ttk0004.MergeFrom(outBlock_yz.Tables[0].Rows[0]);
				}
				ttksm13["CO2_COE"] = ttk0004["CO2_COE"].ToDecimal();
				ttksm13["CO2_WT"] = ttksm13["CO2_COE"] * ttksm13["DEVO_WT"].ToDecimal();
				ttksm13["REC_REVISOR"] = s.userid;
				ttksm13["REC_REVISE_TIME"] = dateNow;
				ttksm13.TrimOrBlank();
				ttksm13.Insert();

			}
		}

		if (bcls_rec->Tables.Contains("DEL_S"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL_S"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL_S"].Rows.get_Count(); i++)
			{
				ttksm13.Reset();
				ttksm13.MergeFrom(bcls_rec->Tables["DEL_S"].Rows[i]);
				ttksm13.Delete("SEQ_NO_RECIPE,MAT_CODE");
			}
		}


	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = -1;
		doFlag = -1;
	}
	return doFlag;
}