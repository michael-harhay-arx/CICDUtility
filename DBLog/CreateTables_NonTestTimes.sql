BEGIN TRANSACTION;

CREATE TABLE "TableName_Panel_SN" (
	"ID"	INTEGER NOT NULL,
	"SerialNumber"	TEXT NOT NULL DEFAULT (strftime('%Y%m%d%H%M%f', 'now', 'localtime')) UNIQUE,
	PRIMARY KEY("ID" AUTOINCREMENT)
);

CREATE TABLE "TableName_NonTestSteps" (
	"ID"	INTEGER NOT NULL,
	"NonTestStepName"	TEXT NOT NULL UNIQUE,
	PRIMARY KEY("ID" AUTOINCREMENT)
);

CREATE TABLE "TableName_NonTestTimes_Batch" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	"BatchNum" INTEGER NOT NULL,
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_Panel_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_NonTestTimes_UUT" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	"BatchNum" INTEGER DEFAULT 0,
	"NestNum" INTEGER NOT NULL,
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_NonTestNotes_Batch" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	"BatchNum" INTEGER NOT NULL,
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_Panel_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE "TableName_NonTestNotes_UUT" (
	"SerialNumber"	TEXT NOT NULL,
	"Date"	NUMERIC DEFAULT (date('now', 'localtime')),
	"Time"	NUMERIC DEFAULT (strftime('%H:%M:%f','now', 'localtime')),
	"BatchNum" INTEGER DEFAULT 0,
	"NestNum" INTEGER NOT NULL,
	FOREIGN KEY("SerialNumber") REFERENCES "TableName_UUT_SN"("SerialNumber") ON UPDATE CASCADE ON DELETE SET NULL
);

END;