#
# Regular cron jobs for the ng-graft package
#
0 4	* * *	root	[ -x /usr/bin/ng-graft_maintenance ] && /usr/bin/ng-graft_maintenance
