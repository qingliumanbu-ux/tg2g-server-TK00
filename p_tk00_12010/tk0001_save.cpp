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
BM2F_ENTERACE(tk0001_save)

int f_tk0001_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	int rowCount = 0;
	CString v_proc_div = "";
	int i = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{		
		CDbCommand cmd(conn);

		CModel ttk0001("TTK0001");

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
			ttk0001.Reset();
			// 取得单行传入信息 
			ttk0001.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (v_proc_div == "I")
			{				
				ttk0001["REC_CREATOR"] = s.userid;
				ttk0001["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

				if (ttk0001["MAT_CODE"].ToString().Trim() == "" )
				{
					strcpy(s.msg, "请输入物料代码");
					s.flag = -1;
					return -1;
				}
				ttk0001.TrimOrBlank();
				ttk0001.Insert();
			}
			else if (v_proc_div == "U")
			{
				ttk0001["REC_REVISOR"] = s.userid;
				ttk0001["REC_REVISE_TIME"] = dateNow;
				ttk0001.Update("REC_REVISOR,REC_REVISE_TIME,MAT_NAME,TYPE_CODE,TYPE_DESC,UNIT,MAT_NAME_T,SEQ_NO,TYPE_CODE1", "MAT_CODE");	
			}
			else if (v_proc_div == "D")
			{
				ttk0001.Delete("MAT_CODE"); 
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