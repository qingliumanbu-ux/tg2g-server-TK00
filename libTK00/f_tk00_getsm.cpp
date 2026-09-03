/*=========================================================================
//程序名称:     f_tk00_getsm
//隶属子系统:   TK
//产品名称:     BM2PES
//创建人员:     ZHOULI
//创建时间:     2012-11-26
//修改人员:   获取生产实绩信息  
//修改日期:     
//=========================================================================*/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
BM2_FUNCTION_EXPORT
int f_tk00_getco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk00_getsm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int i = 0;
	int doFlag = 0;

	CString sqlstr = "";
	CString stat_date = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel ttk0004("TTK0004");

	// 创建电文处理对象
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{

		stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);

		Log::Trace("", "", "stat_date={0}", stat_date);

		sqlstr = " delete from ttksm01"
			" where stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = "insert into ttksm01(REC_CREATOR,REC_CREATE_TIME,STAT_DATE, HEAT_NO, PONO, ST_NO, PROD_TIME, LADLE_NO, CAST_DIV_NO, TD_NO_1"
			",BOF_TIME,AOD_TIME,LF_TIME,RH_TIME,VOD_TIME,LTS_TIME,CC_TIME,DEV_CODE_ROUTE,BACKLOG_EA"
			")"
			" SELECT @rec_creator,@rec_create_time,STAT_DATE, HEAT_NO, PONO, ST_NO, substr(RECV_MAT_TIME,1,8), LADLE_NO, CAST_DIV_NO, TD_NO_1 "
			" ,nvl((select sum(DURATION_TIME) from tmmsmgy06 where dev_code like 'B%' and  heat_no = t.heat_no),0)"
			" ,nvl((select sum(DURATION_TIME) from tmmsmgy06 where dev_code like 'A%' and  heat_no = t.heat_no),0)"
			" ,nvl((select sum(DURATION_TIME) from tmmsmgy06 where dev_code like 'F%' and  heat_no = t.heat_no),0)"
			" ,nvl((select sum(DURATION_TIME) from tmmsmgy06 where dev_code like 'R%' and  heat_no = t.heat_no),0)"
			" ,nvl((select sum(DURATION_TIME) from tmmsmgy06 where dev_code like 'V%' and  heat_no = t.heat_no),0)"
			" ,nvl((select sum(DURATION_TIME) from tmmsmgy06 where dev_code like 'S%' and  heat_no = t.heat_no),0)"
			" ,nvl((select sum(DURATION_TIME) from tmmsmgy06 where dev_code like 'C%' and  heat_no = t.heat_no),0)"
			",nvl((select listagg(DEV_CODE,',') within group(order by start_time)  from tmmsmgy06 where heat_no=t.heat_no),' ')"
			" ,nvl((select SUBSTR(ROUTELIST,5,LENGTH(ROUTELIST)-4)||'C' from tpssm01 t2 where t.pono = t2.pono),' ')  BACKLOG_EA"
			" FROM TMMSMGY05 t"
			" WHERE STAT_DATE = @stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " delete from ttksm02"
			" where stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = "insert into ttksm02(REC_CREATOR,REC_CREATE_TIME,STAT_DATE, HEAT_NO, ST_NO, EQU_NO,MAT_CODE,  WT,DEVO_WT, lot_no,COST_CENTER)"
			" SELECT @rec_creator,@rec_create_time,STAT_DATE, HEAT_NO, ST_NO, DEV_CODE,MAT_CODE,  SUM(DEVO_WT)/1000 DEVO_WT,SUM(DEVO_WT)/1000, LOT_NO "
			" ,nvl((SELECT CODE_DESC_1_CONTENT FROM TEP0002 t2 WHERE  CODE_DESC_5_CONTENT = t1.DEV_CODE AND CODE_CLASS = 'MMDZ' and rownum=1),' ')"
			" from ("
			" SELECT STAT_DATE, HEAT_NO, ST_NO, DEV_CODE,MAT_CODE,  SUM(OUT_STOCK_WT) DEVO_WT, LOT_NO "
			" FROM TMMSM56 t1"
			" WHERE 1=1"
			" and mat_code in (select mat_code from tmmsm50 where send_flag !='1')"
			" AND exists(select 1 from tmmsmgy05 t2 where t2.heat_no=t1.heat_no and t2.STAT_DATE = @stat_date)"
			" GROUP BY STAT_DATE, HEAT_NO, ST_NO,DEV_CODE, MAT_CODE,  LOT_NO "
			" having SUM(OUT_STOCK_WT)!=0 "
			" union all"
			" SELECT STAT_DATE, HEAT_NO, ST_NO, DEV_CODE,MAT_CODE,  SUM(DEVO_WT) DEVO_WT, LOT_NO "
			" FROM TMMSM2A_SEND	 t1"
			" WHERE 1 = 1	"
			" and RTN_FLAG != '1' and SEND_FLAG = '1'	"
			" and HANDLE_DIV = 'F' "
			" AND exists(select 1 from tmmsmgy05 t2 where t2.heat_no=t1.heat_no and t2.STAT_DATE = @stat_date)"
			" GROUP BY STAT_DATE, HEAT_NO, ST_NO,DEV_CODE, MAT_CODE,  LOT_NO "
			" having SUM(DEVO_WT) != 0	"
			" ) t1"
			" GROUP BY STAT_DATE, HEAT_NO, ST_NO,DEV_CODE, MAT_CODE,  LOT_NO "
			" having SUM(DEVO_WT) != 0	"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 		


		//转炉氧气、氮气
		sqlstr = " delete from ttksm05"
			" where stat_date=@stat_date"
			;
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 	

		//针对氧气、氮气、氩气进行精准到到炉次
			sqlstr = "insert into ttksm05(STAT_DATE, l2_proc_no, dev_code, MAT_CODE,  WT)"
				" select  @stat_date,l2_proc_no, dev_code, MAT_CODE, WT"
				" from ("
				" SELECT l2_proc_no, dev_code, '59400' as MAT_CODE, BLOW_NUMBER as PROC_COUNT, BLOW_DURATION as PROC_TIME, OXYGEN_FINAL/1000 wt "
				" FROM TMMSM21 t"
				" WHERE 1 = 1"
				" and OXYGEN_FINAL!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.l2_proc_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//转炉氮气
				" union all "
				" SELECT l2_proc_no, dev_code, '59401' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, TOTAL_N2_CONS/1000 wt"
				" FROM TMMSM21 t"
				" WHERE 1 = 1"
				" and TOTAL_N2_CONS!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.l2_proc_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				" union all"
				//转炉氩气
				" SELECT l2_proc_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, AR_SUM_COMSUME/1000 wt "
				" FROM TMMSM21  t"
				" WHERE 1 = 1"
				" and ar_sum_comsume!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.l2_proc_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//电炉电
				" union all"
				" SELECT l2_proc_no, dev_code, '59103' as MAT_CODE, heat_count as PROC_COUNT, bil_melt_time as PROC_TIME, power_consume as wt"
				" FROM TMMSM20 t"
				" WHERE 1 = 1"
				" and power_consume!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//电炉氧气
				" union all"
				" SELECT l2_proc_no, dev_code, '59400' as MAT_CODE, 0 as PROC_COUNT, blow_duration as PROC_TIME, oxygen_final/1000 as wt"
				" FROM TMMSM20 t"
				" WHERE 1 = 1"
				" and oxygen_final!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//LF精炼氩气
				" union all"
				" select l2_proc_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, AR_SUM_COMSUME/1000 wt"
				" from TMMSM24 t"
				" WHERE 1 = 1"
				" and AR_SUM_COMSUME!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//LF精炼电
				" union all"
				" select l2_proc_no, dev_code, '59103' as MAT_CODE, HEAT_COUNT as PROC_COUNT, BIL_MELT_time as PROC_TIME, power_consume wt	"
				" from TMMSM24 t"
				" WHERE 1 = 1"
				" and power_consume!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//rh精炼氩气
				" union all"
				" select l2_proc_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, AR_SUM_COMSUME/1000 wt"
				" from TMMSM23 t"
				" WHERE 1 = 1"
				" and AR_SUM_COMSUME!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//rh精炼氮气
				" union all"
				" select l2_proc_no, dev_code, '59401' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, N_SUM_COMSUME/1000 wt "
				" from TMMSM23 t"
				" WHERE 1 = 1 "
				" and N_SUM_COMSUME!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)" 				
				//VOD氩气
				" union all"
				" select l2_proc_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, AR_SUM_COMSUME/1000 wt	"
				" from TMMSM25 t"
				" WHERE 1 = 1"
				" and AR_SUM_COMSUME!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//VOD氧气
				" union all"
				" select l2_proc_no, dev_code, '59400' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, oxygen_final/1000 wt"
				" from TMMSM25 t"
				" WHERE 1 = 1"
				" and oxygen_final!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)" 			
				//AOD氩气
				" union all"
				" select l2_proc_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, total_ar_cons/1000 wt"
				" from TMMSM27 t"
				" WHERE 1 = 1"
				" and total_ar_cons!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//AOD 氮气
				" union all"
				" select l2_proc_no, dev_code, '59401' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, total_n2_cons/1000 wt "
				" from TMMSM27 t"
				" WHERE 1=1"
				" and total_n2_cons!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//AOD氧气
				" union all"
				" select l2_proc_no, dev_code, '59400' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, oxygen_final/1000 wt "
				" from TMMSM27 t"
				" WHERE 1=1"
				" and oxygen_final!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				//LTS氩气
				" union all"
				" select l2_proc_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, AR_BLOW_TIME as PROC_TIME, ar_sum_comsume/1000 wt"
				" from TMMSM26 t"
				" WHERE 1 = 1"
				" and ar_sum_comsume!=0"
				" and exists (select 1 from tmmsmgy05 t2 where t2.heat_no=t.l2_proc_no and  t2.STAT_DATE = @stat_date)"
				" )"
				; 
				Log::Trace("", "", "sqlstr={0} ", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

		//插入能源消耗
		sqlstr = "insert into ttksm02(STAT_DATE, HEAT_NO, ST_NO, EQU_NO,MAT_CODE,  WT,DEVO_WT, COST_CENTER)"
			" select @stat_date,t.heat_no,nvl(t3.st_no,' '),t.dev_code,t.mat_code,decode(nvl(all_wt,0),0,0,round(use_wt*mat_act_wt/all_wt,0)) ,decode(nvl(all_wt,0),0,0,round(use_wt*mat_act_wt/all_wt,0))"
			" ,nvl((SELECT CODE_DESC_1_CONTENT FROM TEP0002 t2 WHERE  CODE_DESC_5_CONTENT = t.DEV_CODE AND CODE_CLASS = 'MMDZ' and rownum=1),' ')"
			" from  ("
			" select t1.heat_no,t1.l2_proc_no,t1.dev_code,t1.HEAT_COUNT,t2.mat_code"
			",case when t1.HEAT_COUNT=0 or t1.HEAT_COUNT=1 then t2.wt else round(t2.wt/t1.HEAT_COUNT,0) end use_wt from tmmsmgy06 t1 "
			" left join ttksm05 t2 on t1.l2_proc_no=t2.l2_proc_no and t1.dev_code =t2.dev_code and t2.STAT_DATE = @stat_date"
			" where  nvl(t2.mat_code,' ')!=' ' and t1.STAT_DATE = @stat_date"
			" ) t "
			" left join (select heat_no,st_no,sum(mat_act_wt) mat_act_wt from tmmsm56b where STAT_DATE = @stat_date group by heat_no,st_no) t3 on t.heat_no=t3.heat_no "
			" left join (select heat_no,sum(mat_act_wt) all_wt from tmmsm56b where STAT_DATE = @stat_date group by heat_no) t4 on t.heat_no=t4.heat_no"
			;
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
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
		cmd_inq.Parameters.Set("stat_date", stat_date);
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
				ttk0004.MergeFrom(outBlock_yz.Tables[0].Rows[0]);
				sqlstr = " update ttksm02 set CO2_COE= @co2_coe"
					",CO2_WT = round(wt*@co2_coe,6)"
					" ,CO2_COE_UNIT = @co2_coe_unit"
					",CO2_COE1= @co2_coe1"
					",CO2_WT1 = round(wt*@co2_coe1,6)"
					",CO2_COE2= @co2_coe2"
					",CO2_WT2 = round(wt*@co2_coe2,6)"
					",mat_name = @mat_name"
					" where 1=1"
					" and mat_code = @mat_code"
					" and stat_date=@stat_date"
					;
				cmd_inq_s.SetCommandText(sqlstr);
				cmd_inq_s.Parameters.Set("stat_date", stat_date);
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

		//更新铁水成分下
		sqlstr = " update ttksm01 t1 set (IRON_TEMP,IRON_C,IRON_SI,IRON_MN,IRON_P,IRON_S,OUT_STEEL_TEMP) = (select IRON_TEMP,IRON_C,IRON_SI,IRON_MN,IRON_P,IRON_S,OUT_STEEL_TEMP from tmmsm21 t2 where t1.heat_no =t2.l2_proc_no)"
			" where exists (select 1 from tmmsm21 t2 where t1.heat_no =t2.l2_proc_no)"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新铁水成分下
		sqlstr = " UPDATE ttksm01 T1 SET (IRON_TEMP,IRON_C,IRON_SI,IRON_MN,IRON_P,IRON_S) =  "
			"(SELECT IRON_TEMP, IRON_C, IRON_SI, IRON_MN, IRON_P, IRON_S FROM tmmsm21 WHERE L2_PROC_NO IN(SELECT L2_PROC_NO FROM TMMSMGY06 T2 WHERE  DEV_CODE like 'B%' AND t1.heat_no = t2.heat_no))"
			" WHERE IRON_TEMP = 0 "
			" and EXISTS(SELECT 1 FROM TMMSMGY06 T2 WHERE  DEV_CODE like 'B%' AND t1.heat_no = t2.heat_no) "
			" and stat_date=@stat_date"
		;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update ttksm01 t1 set (IRON_TEMP,IRON_C,IRON_SI,IRON_MN,IRON_P,IRON_S,OUT_STEEL_TEMP) = (select IRON_TEMP,IRON_C,IRON_SI,IRON_MN,IRON_P,IRON_S,OUT_STEEL_TEMP from tmmsm27 t2 where t1.heat_no =t2.l2_proc_no)"
			" where exists (select 1 from tmmsm27 t2 where t1.heat_no =t2.l2_proc_no)"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新钢水重量
		sqlstr = " update ttksm01 t1 set STEEL_WT = (select LADLE_ARRIVE_WT-LADLE_LEAVE_WT from tmmsm31 t2 where t1.heat_no =t2.l2_proc_no)"
			" where exists (select 1 from tmmsm31 t2 where t1.heat_no =t2.l2_proc_no)"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//更新钢水重量
		sqlstr = " update ttksm01 t1 set PROD_WT = (select sum(mat_act_wt) from tmmsm56b t2 where t1.heat_no =t2.heat_no)"
			" where exists (select 1 from tmmsm56b t2 where t1.heat_no =t2.heat_no)"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新钢水重量
		sqlstr = " update ttksm01 t1 set C_DIV = case when substr(st_no,1,1) in ('1','4') then '1' else '2' end"
			" where 1=1"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新钢水重量
		sqlstr = " update ttksm01 t1 set sg_sign = (select SG_GRADE_1 from tqmts0x t2 where t1.st_no=t2.st_no)"
			" where 1=1"
			" and exists(select SG_GRADE_1 from tqmts0x t2 where t1.st_no=t2.st_no)"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 

		//更新路径
		sqlstr = " update ttksm01 t1 set BACKLOG_EA = (select SUBSTR(ROUTELIST,5,LENGTH(ROUTELIST)-4)||'C' from tpssm41 t2 where t1.heat_no = t2.heat_no and rownum=1)"
			" where 1=1"
			" and exists(select SUBSTR(ROUTELIST,5,LENGTH(ROUTELIST)-4) from tpssm41 t2 where t1.heat_no = t2.heat_no)"
			" and   BACKLOG_EA IN (' ','C')"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新路径
		sqlstr = " update ttksm01 t1 set BACKLOG_EA = (select SUBSTR(ROUTELIST,5,LENGTH(ROUTELIST)-4)||'C' from tpssm11 t2 where t1.heat_no = t2.heat_no and rownum=1)"
			" where 1=1"
			" and exists(select SUBSTR(ROUTELIST,5,LENGTH(ROUTELIST)-4) from tpssm11 t2 where t1.heat_no = t2.heat_no)"
			" and  BACKLOG_EA IN (' ','C')"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 	
		


		//更新钢水重量
		sqlstr = " update ttksm02 t1 set (C_DIV,prod_time,BACKLOG_EA) = (select C_DIV,prod_time,BACKLOG_EA from ttksm01 t2 where t1.heat_no =t2.heat_no)"
			" where exists (select 1 from ttksm01 t2 where t1.heat_no =t2.heat_no)"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update ttksm02 t1 set (mat_name,MAT_CODE_T) = (select mat_name,MAT_CODE_T from ttk0001 t2 where t1.mat_code =t2.mat_code)"
			" where exists (select 1 from ttk0001 t2 where t1.mat_code =t2.mat_code)"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update ttksm01 t1 set (CO2_WT,CO2_WT1,CO2_WT2) = (select SUM(CO2_WT),SUM(CO2_WT1),SUM(CO2_WT2) from ttksm02 t2 where t1.heat_no =t2.heat_no)"
			" where exists (select 1 from ttksm02 t2 where t1.heat_no =t2.heat_no)"
			" and stat_date=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		// STEEL_TYPE（C碳钢 / S不锈钢 / Ni镍钢 / Cr硌钢）判断依据，st_no 第一位：不锈钢（2，3，5 ）碳钢（1，4），第二位 铬钢(F, M)、镍钢（A, D）


	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.msg, (const char*)str, sizeof(s.msg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
