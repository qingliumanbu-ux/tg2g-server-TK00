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
BM2F_ENTERACE(tksm10_save)
int f_tksm10_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

		CModel ttksm11("TTKSM11");
		CModel ttksm12("TTKSM12");

		proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttksm11.Reset();
			ttksm11.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (proc_div == "I")
			{
				ttksm11["ST_NO"] = bcls_rec->Tables[0].Rows[i]["ST_NO"].ToString();
				ttksm11["WHOLE_BACKLOG"] = bcls_rec->Tables[0].Rows[i]["DEV_CODE"].ToString();

				ttksm11["REC_CREATOR"] = s.userid;
				ttksm11["REC_CREATE_TIME"] = dateNow;
				cmd_inq.SetCommandText("select  nvl(MAX(SEQ_NO),0)+1 from ttksm11");
				ttksm11["SEQ_NO"] = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				ttksm11.Insert();

			}
			else if (proc_div == "D")
			{
				ttksm11.Delete("SEQ_NO");
				ttksm12["SEQ_NO"] = ttksm11["SEQ_NO"];
				ttksm12.Delete("SEQ_NO");
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