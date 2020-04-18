select lose_match.win_lose, count(lose_match.match_id) as cnt
from (
  select 'lose' as win_lose, m.match_id, avg(s.longesttimespentliving) as avg_ltsl
  from match_info m, participant p, stat s
  where m.match_id = p.match_id and p.player_id = s.player_id and s.win = 0
  group by m.match_id
) as lose_match
where lose_match.avg_ltsl >= 20 * 60
group by lose_match.win_lose
union
select win_match.win_lose, count(win_match.match_id) as cnt
from (
  select 'win' as win_lose, m.match_id, avg(s.longesttimespentliving) as avg_ltsl
  from match_info m, participant p, stat s
  where m.match_id = p.match_id and p.player_id = s.player_id and s.win = 1
  group by m.match_id
) as win_match
where win_match.avg_ltsl >= 20 * 60
group by win_match.win_lose;
