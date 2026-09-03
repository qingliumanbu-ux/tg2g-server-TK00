/// <summary>
/// 功能说明: 物料编码对应成本科目保存
/// </summary>
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company:   上海宝信软件股份有限公司
/// Author:    
/// Version:   1.0
/// History:
///		

#include "stdafx.h"
// Service 入口
BM2F_ENTERACE(tkbs01_save)

int f_tkbs01_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	int rowCount = 0;
	CString v_proc_div = "";
	int i = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{		
		CDbCommand cmd(conn);

		CModel ttk0002("TTK0002");

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
			ttk0002.Reset();
			// 取得单行传入信息 
			ttk0002.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (v_proc_div == "I")
			{				
				ttk0002["REC_CREATOR"] = s.userid;
				ttk0002["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
							
				ttk0002.TrimOrBlank();
				ttk0002.Insert();
			}
			else if (v_proc_div == "U")
			{
				ttk0002["REC_REVISOR"] = s.userid;
				ttk0002["REC_REVISE_TIME"] = dateNow;
				ttk0002.Update("REC_REVISOR,REC_REVISE_TIME,AMOUNT_TAX", "VALID_TIME");	
			}
			else if (v_proc_div == "D")
			{
				ttk0002.Delete("VALID_TIME"); 
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