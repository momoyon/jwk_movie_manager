#define COMMONLIB_REMOVE_PREFIX
#define COMMONLIB_IMPLEMENTATION
#include "commonlib.h"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

/// :utility ///
#define TODO(msg) ASSERT(false, "TODO: "msg)

static inline const char* get_os() {
    #if defined(_WIN32) || defined(_WIN64)
        return "Windows";
    #elif defined(__APPLE__) || defined(__MACH__)
        return "macOS";
    #elif defined(__linux__)
        return "Linux";
    #elif defined(__unix__)
        return "Unix";
    #else
        return "Unknown OS";
    #endif
}

////////////////////

void usage(FILE *f, const char *program);
void help(FILE *f, const char *program);

/// :datetime ///
#define DATETIME_FMT "%02d/%02d/%04d %02d:%02d"
#define DATETIME_ARG(dt) (dt).day, (dt).month, (dt).year, (dt).hour, (dt).minute

typedef struct Datetime Datetime;

struct Datetime {
	int day, month, year;
	int second, minute, hour;
};

// NOTE: DateTime string format: DD/MM/YYYY HH:mm
Datetime get_current_datetime(void);
bool parse_datetime(const char *str, Datetime *dt);

int main(int argc, char **argv) {
	ASSERT(strcmp(get_os(), "Windows") == 0, "This program only supports Windows at the moment!");
	const char *program = shift_args(argv, argc);

	const char *schedule_config_filepath = "./schedule.config";

	if (argc <= 0) {
		usage(stdout, program);
		return 0;
	}

	// log_debug("Current datetime: "DATETIME_FMT, DATETIME_ARG(get_current_datetime()));

	while (argc > 0) {
		const char *arg = shift_args(argv, argc);

		if (strcmp(arg, "add") == 0) {
			if (argc <= 0) {
				log_error("`add` subcommand expects <movie> and <date> as arguments!");
				return 1;
			}

			const char *movie = shift_args(argv, argc);
			if (argc <= 0) {
				log_error("`add` subcommand expects <movie> and <date> as arguments!");
				return 1;
			}
			const char *date = shift_args(argv, argc);

			Datetime dt = {0};
			if (!parse_datetime(date, &dt)) {
				return 1;
			}

			// log_debug("Scheduling movie `%s` for date "DATETIME_FMT, movie, DATETIME_ARG(dt));
			TODO("add subcmd");
		} else if (strcmp(arg, "help") == 0) {
			help(stdout, program);
			return 0;
		} else {
			usage(stderr, program);
			log_error("Unrecognized subcommand `%s`", arg);
			return 1;
		}
	}
}
void usage(FILE *f, const char *program) {
	fprintf(f, "Usage: %s <subcommand>\n", program);
}

void help(FILE *f, const char *program) {
	usage(f, program);
	fprintf(f, "\n");
	fprintf(f, "Subcommands:\n");
	fprintf(f, "  help               -    Prints this help message.\n");
	fprintf(f, "  add <movie> <date> -    Schedules <movie> for date <date>.\n");
}

/// :datetime ///

Datetime get_current_datetime(void) {
	Datetime dt = {0};
#if defined(_WIN32) || defined(_WIN64)
	SYSTEMTIME systime = {0};
	GetSystemTime(&systime);
	GetLocalTime(&systime);
	dt.day = systime.wDay;
	dt.month = systime.wMonth;
	dt.year = systime.wYear;
	dt.hour = systime.wHour;
	dt.minute = systime.wMinute;
#endif

	return dt;
}

bool parse_datetime(const char *str, Datetime *dt) {
	if (str == NULL) {
		log_error("Failed to parse datetime: empty string provided!");
		return false;
	}
	String_view sv = SV(str);

	sv_trim(&sv);

	String_view day = sv_lpop_until_char(&sv, '/');
	sv_lremove(&sv, 1); // remove /
	sv_ltrim(&sv);

	if (sv.count <= 0) {
		log_error("Failed to parse datetime: make sure its in the format DD/MM/YYYY [HH:mm]!");
		return false;
	}

	String_view month = sv_lpop_until_char(&sv, '/');
	sv_lremove(&sv, 1); // remove /
	sv_ltrim(&sv);

	if (sv.count <= 0) {
		log_error("Failed to parse datetime: make sure its in the format DD/MM/YYYY [HH:mm]!");
		return false;
	}

	String_view year = {0};
	if (sv_contains_char(sv, ' ')) {
		year = sv_lpop_until_char(&sv, ' ');
		sv_lremove(&sv, 1); // remove /
		sv_ltrim(&sv);
	} else {
		year = sv;
		sv.count -= year.count;
	}

	// HH:mm also provided
	if (sv.count > 0) {
		String_view hour = sv_lpop_until_char(&sv, ':');
		sv_lremove(&sv, 1); // remove :
		sv_ltrim(&sv);

		if (sv.count <= 0) {
			log_error("Failed to parse datetime: make sure its in the format DD/MM/YYYY [HH:mm]!");
			return false;
		}

		String_view minute = sv;

		sv.count -= minute.count;

		int hour_count = -1;
		int HH = sv_to_int(hour, &hour_count, 10);
			if (hour_count == -1) {
			log_error("Failed to parse datetime: failed to parse HH (could not convert to integer!) make sure its in the format DD/MM/YYYY [HH:mm]!");
			return false;
		}
		dt->hour = HH;

		int minute_count = -1;
		int mm = sv_to_int(minute, &minute_count, 10);
		if (minute_count == -1) {
			log_error("Failed to parse datetime: failed to parse mm (could not convert to integer!) make sure its in the format DD/MM/YYYY [HH:mm]!");
			return false;
		}
		dt->minute = mm;

		if (dt->hour > 23 || dt->hour < 0) {
			log_error("Failed to parse time: Hour should be in the range 0 ~ 23");
			return false;
		}

		if (dt->minute > 59 || dt->minute < 0) {
			log_error("Failed to parse time: Minute should be in the range 0 ~ 59");
			return false;
		}
	}

	int day_count = -1;
	int DD = sv_to_int(day, &day_count, 10);
	if (day_count == -1) {
		log_error("Failed to parse datetime: failed to parse DD (could not convert to integer!) make sure its in the format DD/MM/YYYY [HH:mm]!");
		return false;
	}
	dt->day = DD;

	int month_count = -1;
	int MM = sv_to_int(month, &month_count, 10);
	if (month_count == -1) {
		log_error("Failed to parse datetime: failed to parse MM (could not convert to integer!) make sure its in the format DD/MM/YYYY [HH:mm]!");
		return false;
	}
	dt->month = MM;

	int year_count = -1;
	int YYYY = sv_to_int(year, &year_count, 10);
	if (year_count == -1) {
		log_error("Failed to parse datetime: failed to parse YYYY (could not convert to integer!) make sure its in the format DD/MM/YYYY [HH:mm]!");
		return false;
	}
	dt->year = YYYY;

	// log_debug("Datetime parsed: "DATETIME_FMT, DATETIME_ARG(*dt));

	return true;
}
