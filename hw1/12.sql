(
  select 'Flash on [D]' as Flash_Position, dw.win as win, dl.lose as lose,
    dw.win / dl.lose as win_ratio, dw.gold / dw.win as win_avg_gold, dl.gold / dl.lose as lose_avg_gold
  from (
    select count(*) as win, sum(s.goldearned) as gold
    from participant p, stat s
    where p.player_id = s.player_id and p.ss1 like 'Flash%' and s.win = 1
  ) as dw, (
    select count(*) as lose, sum(s.goldearned) as gold
    from participant p, stat s
    where p.player_id = s.player_id and p.ss1 like 'Flash%' and s.win = 0
  ) as dl
) union all (
  select 'Flash on [F]' as Flash_Position, dw.win as win, dl.lose as lose,
    dw.win / dl.lose as win_ratio, dw.gold / dw.win as win_avg_gold, dl.gold / dl.lose as lose_avg_gold
  from (
    select count(*) as win, sum(s.goldearned) as gold
    from participant p, stat s
    where p.player_id = s.player_id and p.ss2 like 'Flash%' and s.win = 1
  ) as dw, (
    select count(*) as lose, sum(s.goldearned) as gold
    from participant p, stat s
    where p.player_id = s.player_id and p.ss2 like 'Flash%' and s.win = 0
  ) as dl
);
