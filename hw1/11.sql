(
  select 'Flash + Ignite' as SS, iw.win as win, il.lose as lose,
    iw.win / il.lose as win_ratio, iw.gold / iw.win as win_avg_gold, il.gold / il.lose as lose_avg_gold
  from (
    select count(*) as win, sum(s.goldearned) as gold
    from participant p, stat s
    where p.player_id = s.player_id and p.position like 'TOP%'
      and ((p.ss1 like 'Flash%' and p.ss2 like 'Ignite%') or (p.ss2 like 'Flash%' and p.ss1 like 'Ignite%'))
      and s.win = 1
  ) as iw, (
    select count(*) as lose, sum(s.goldearned) as gold
    from participant p, stat s
    where p.player_id = s.player_id and p.position like 'TOP%'
      and ((p.ss1 like 'Flash%' and p.ss2 like 'Ignite%') or (p.ss2 like 'Flash%' and p.ss1 like 'Ignite%'))
      and s.win = 0
  ) as il
) union all (
  select 'Flash + Teleport' as SS, tw.win as win, tl.lose as lose,
    tw.win / tl.lose as win_ratio, tw.gold / tw.win as win_avg_gold, tl.gold / tl.lose as lose_avg_gold
  from (
    select count(*) as win, sum(s.goldearned) as gold
    from participant p, stat s
    where p.player_id = s.player_id and p.position like 'TOP%'
      and ((p.ss1 like 'Flash%' and p.ss2 like 'Teleport%') or (p.ss2 like 'Flash%' and p.ss1 like 'Teleport%'))
      and s.win = 1
  ) as tw, (
    select count(*) as lose, sum(s.goldearned) as gold
    from participant p, stat s
    where p.player_id = s.player_id and p.position like 'TOP%'
      and ((p.ss1 like 'Flash%' and p.ss2 like 'Teleport%') or (p.ss2 like 'Flash%' and p.ss1 like 'Teleport%'))
      and s.win = 0
  ) as tl
);
