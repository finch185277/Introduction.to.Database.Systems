select count(distinct SUBSTRING_INDEX(m.version, '.', 2)) as cnt
from match_info as m;
