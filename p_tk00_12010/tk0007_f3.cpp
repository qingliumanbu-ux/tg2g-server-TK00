/// <summary>
/// 功能说明: 碳效等级阈值保存
/// </summary>
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company:   上海宝信软件股份有限公司
/// Author:    
/// Version:   1.0
/// History:
///		

#include "stdafx.h"
// Service 入口
BM2F_ENTERACE(tk0007_f3)

int f_tk0007_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	int rowCount = 0;
	try
	{
		CDbCommand cmd(conn);

		CModel ttk0007("TTK0007");

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				ttk0007.Reset();
				// 取得单行传入信息 
				ttk0007.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				ttk0007["REC_CREATOR"] = s.userid;
				ttk0007["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

				if (ttk0007["CO2_EFF_LEV"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入碳效等级！");
					s.flag = -1;
					return -1;
				}
				Log::Trace("", __FUNCTION__, "CO2_EFF_LEV:", ttk0007["CO2_EFF_LEV"].ToString().Trim());
				Log::Trace("", __FUNCTION__, "ROWCOUNT:", bcls_rec->Tables["ADD"].Rows.get_Count());
				ttk0007.TrimOrBlank();
				ttk0007.Insert();
			}
		}
		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				ttk0007.Reset();
				// 取得单行传入信息 
				ttk0007.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				ttk0007["REC_REVISOR"] = s.userid;
				ttk0007["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

				if (ttk0007["CO2_EFF_LEV"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入碳效等级！");
					s.flag = -1;
					return -1;
				}
				//执行修改
				int rowAffected = ttk0007.Update("REC_REVISOR,REC_REVISE_TIME,CO2_EFF_LEV_VALUE", "CO2_EFF_LEV");
				if (rowAffected<0)
				{
					strcpy(s.msg, "记录未找到！");
					s.flag = -1;
					return -1;
				}
			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				// 取得单行传入信息 
				ttk0007.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
				//根据主键删除			
				ttk0007.Delete("*");

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