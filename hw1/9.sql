select ps.version as version, ps.win_cnt as win_cnt, ps.lose_cnt as lose_cnt, win_cnt / lose_cnt as win_ratio
from
(
  (
    select SUBSTRING_INDEX(wm.version, '.', 2) as version, count(wm.match_id) as win_cnt, 0 as lose_cnt
    from (
      select wps1.match_id, wps1.win
      from (
        select wp1.match_id, ws1.win, wp1.champion_id from participant wp1, stat ws1 where wp1.player_id = ws1.player_id
      ) as wps1, (
        select wp2.match_id, ws2.win, wp2.champion_id from participant wp2, stat ws2 where wp2.player_id = ws2.player_id
      ) as wps2
      where wps1.match_id = wps2.match_id and wps1.win = wps2.win and wps1.champion_id = 17 and wps2.champion_id = 64
    ) as wps, match_info wm
    where wm.match_id = wps.match_id and wps.win = 1
    group by SUBSTRING_INDEX(wm.version, '.', 2)
  ) union all
  (
    select SUBSTRING_INDEX(lm.version, '.', 2) as version, 0 as win_cnt, count(lm.match_id) as lose_cnt
    from (
      select lps1.match_id, lps1.win
      from (
        select lp1.match_id, ls1.win, lp1.champion_id from participant lp1, stat ls1 where lp1.player_id = ls1.player_id
      ) as lps1, (
        select lp2.match_id, ls2.win, lp2.champion_id from participant lp2, stat ls2 where lp2.player_id = ls2.player_id
      ) as lps2
      where lps1.match_id = lps2.match_id and lps1.win = lps2.win and lps1.champion_id = 17 and lps2.champion_id = 64
    ) as lps, match_info lm
    where lm.match_id = lps.match_id and lps.win = 0
    group by SUBSTRING_INDEX(lm.version, '.', 2)
  )
) as ps
group by version;
