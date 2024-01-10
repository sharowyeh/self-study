#pragma once

// interest thing from https://stackoverflow.com/questions/457577/catching-access-violation-exceptions (look down to vs2015 ver)
// NOTE: it is customize the log message for issue evaluation or for fun,
//   but is NOT to ignore exceptions from access violation or stack frame issues during process running
int access_violation();
