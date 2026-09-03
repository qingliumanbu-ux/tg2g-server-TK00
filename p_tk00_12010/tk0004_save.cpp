/// <summary>
/// 功能说明: 排放因子保存
/// </summary>
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company:   上海宝信软件股份有限公司
/// Author:    
/// Version:   1.0
/// History:
///		

#include "stdafx.h"
// Service 入口
BM2F_ENTERACE(tk0004_save)

int f_tk0004_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	int rowCount = 0;
	try
	{		
		CDbCommand cmd(conn);

		CModel ttk0004("TTK0004");

		// 修改明细
		if (bcls_rec->Tables.Contains("MODIFY"))
		{
			for (int i = 0; i < bcls_rec->Tables["MODIFY"].Rows.get_Count(); i++)
			{
				ttk0004.Reset();
				// 取得单行传入信息 
				ttk0004.MergeFrom(bcls_rec->Tables["MODIFY"].Rows[i]);
				ttk0004["REC_REVISOR"] = s.userid;
				ttk0004["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

				if (ttk0004["MAT_CODE"].ToString().Trim() == "" ||  ttk0004["VALID_TIME"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入物料代码、生效时间");
					s.flag = -1;
					return -1;
				}
				//执行修改
				int rowAffected = ttk0004.Update("REC_REVISOR,REC_REVISE_TIME,C_YIELD,C_STD,CO2_COE,CO2_COE_UNIT,HOT_VAL,HOT_VAL_UNIT,CO2_COE1,CO2_COE2","MAT_CODE,DATA_FROM,C_TYPE,VALID_TIME"); 
				if (rowAffected<0)
				{
					strcpy(s.msg, "记录未找到");
					s.flag = -1;
					return -1;
				}
			}
		}
		// 新增明细
		if (bcls_rec->Tables.Contains("ADD"))
		{
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				ttk0004.Reset();
				// 取得单行传入信息 
				ttk0004.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				ttk0004["REC_CREATOR"] = s.userid;
				ttk0004["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

				if (ttk0004["MAT_CODE"].ToString().Trim() == "" || ttk0004["VALID_TIME"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入物料代码、生效时间");
					s.flag = -1;
					return -1;
				}
				ttk0004.TrimOrBlank();
				ttk0004.Insert();
			}
		}
		// 删除操作
		if (bcls_rec->Tables.Contains("DELETE"))
		{
			for (int i = 0; i < bcls_rec->Tables["DELETE"].Rows.get_Count(); i++)
			{
				// 取得单行传入信息 
				ttk0004.MergeFrom(bcls_rec->Tables["DELETE"].Rows[i]);
				//根据主键删除			
				ttk0004.Delete("MAT_CODE,DATA_FROM,C_TYPE,VALID_TIME");

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