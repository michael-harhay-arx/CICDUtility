BEGIN TRANSACTION;

CREATE TABLE "TableName_UUT_SN" (
	"ID"	INTEGER NOT NULL,
	"SerialNumber"	TEXT NOT NULL DEFAULT (strftime('%Y%m%d%H%M%f', 'now', 'localtime')) UNIQUE,
	PRIMARY KEY("ID" AUTOINCREMENT)
);

CREATE TABLE "TableName_TestSteps" (
	"ID"	INTEGER NOT NULL,
	"TestStepName"	TEXT NOT NULL UNIQUE,
	PRIMARY KEY("ID" AUTOINCREMENT)
);

CREATE TABLE "TableName_PassFailLog" (
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	"TestTime"	REAL,
	"FixtureID"	TEXT NOT NULL,
	"NestNum"	INTEGER NOT NULL,
	"SerialNumber"	TEXT NOT NULL,
	"SequencePassFail"	TEXT,
	"FailedTests"	TEXT,
	-- [Replace Here]	>0>"\t\""+Locals.CurrentElement+"\"\tTEXT,\n"
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_TestNum" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	-- [Replace Here]	>0>"\t\""+Locals.CurrentElement+"\"\tTEXT,\n"
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_TestVal" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	-- [Replace Here]	>0>"\t\""+Locals.CurrentElement+"\"\tTEXT,\n"
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_TestTime" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	-- [Replace Here]	>0>"\t\""+Locals.CurrentElement+"\"\tTEXT,\n"
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_TestLimMin" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	-- [Replace Here]	>0>"\t\""+Locals.CurrentElement+"\"\tTEXT,\n"
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_TestLimMax" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	-- [Replace Here]	>0>"\t\""+Locals.CurrentElement+"\"\tTEXT,\n"
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_TestAdditionalNotes" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	-- [Replace Here]	>0>"\t\""+Locals.CurrentElement+"\"\tTEXT,\n"
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

END;