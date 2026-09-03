/// <summary>
/// 功能说明: 钢种工序标准碳排保存
/// </summary>
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company:   上海宝信软件股份有限公司
/// Author:    
/// Version:   1.0
/// History:
///		

#include "stdafx.h"
// Service 入口
BM2F_ENTERACE(tksm10_pf_conn)
int f_tk00_getco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tksm10_pf_conn(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	int rowCount = 0;
	CDecimal  seq_no_pf = 0;
	CString v_proc_div = "";
	int i = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		CDbCommand cmd_inq(conn);

        CModel ttksm12("TTKSM12");
		cmd_inq.SetCommandText("SELECT  * FROM TTKSM13 where SEQ_NO_RECIPE=@SEQ_NO_RECIPE ");
		cmd_inq.Parameters.Set("SEQ_NO_RECIPE", bcls_rec->Tables[0].Rows[0]["SEQ_NO_RECIPE"]);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			ttksm12.Reset();
			cmd_inq.Fetch(ttksm12);

			ttksm12["SEQ_NO"] = bcls_rec->Tables[0].Rows[0]["SEQ_NO"].ToDecimal();
			ttksm12["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"];
			ttksm12["WHOLE_BACKLOG"] = bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG"];
			ttksm12["REC_CREATOR"] = s.userid;
			ttksm12["REC_CREATE_TIME"] = dateNow;
			ttksm12.Insert();
		}
		cmd_inq.Close();

		 

		
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = -1;
		doFlag = -1;
	}
	return doFlag;
}