select count(distinct concat(SUBSTRING_INDEX(m.version, '.', 1), '.', SUBSTRING_INDEX(m.version, '.', 2))) as cnt
from match_info as m;
