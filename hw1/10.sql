select c.champion_name as self_champ_name, r2.win_ratio, r2.self_kda, r2.self_avg_gold, 'Renekton' as enemy_champ_name, r2.enemy_kda, r2.enemy_avg_gold, r2.battle_record
from (
    select r.champion_id, r.win_cnt / r.cnt as win_ratio, (r.sk + r.sa) / r.sd as self_kda, r.sg / r.cnt as self_avg_gold,
    (r.ek + r.ea) / r.ed as enemy_kda, r.eg / r.cnt as enemy_avg_gold, r.cnt as battle_record
  from (
    select p1.champion_id, count(champion_id) as cnt, sum(s1.win) as win_cnt,
      sum(s1.kills) as sk,  sum(s1.deaths) as sd, sum(s1.assists) as sa, sum(s1.goldearned) as sg,
      sum(rene.kills) as ek, sum(rene.deaths) as ed, sum(rene.assists) as ea, sum(rene.goldearned) as eg
    from (
      select p.match_id, s.win, s.kills, s.deaths, s.assists, s.goldearned
      from participant p, stat s
      where p.player_id = s.player_id and p.position like 'TOP%' and p.champion_id = 58
    ) as rene, participant p1, stat s1
    where p1.player_id = s1.player_id and p1.match_id = rene.match_id and s1.win != rene.win and p1.position like 'TOP%'
    group by p1.champion_id
  ) as r
  where r.cnt > 100 and r.sd != 0
  order by win_ratio desc
  limit 5
) as r2, champ c
where r2.champion_id = c.champion_id;
