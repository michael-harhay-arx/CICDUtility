BEGIN TRANSACTION;

DROP VIEW IF EXISTS [vSerialCount];
CREATE VIEW [vSerialCount] AS
SELECT
	[TableName_PassFailLog].[SerialNumber],
	COUNT([TableName_PassFailLog].[SerialNumber]) AS SerialCount
FROM [TableName_PassFailLog]
GROUP BY [SerialNumber]
ORDER BY SerialCount DESC;

DROP VIEW IF EXISTS [vTestNumberLog];
CREATE VIEW [vTestNumberLog] AS
SELECT DISTINCT
	[TableName_PassFailLog].[Date],
	[TableName_PassFailLog].[Time],
	[TableName_PassFailLog].[TestTime],
	[TableName_PassFailLog].[NestNum],
	[TableName_PassFailLog].[SerialNumber],
	[TableName_PassFailLog].[SequencePassFail],
	[TableName_PassFailLog].[FailedTests],
	-- [Replace Here]	>2>"\t[TableName_TestNumber].["+Locals.CurrentElement+"],\n"
FROM [TableName_PassFailLog]
INNER JOIN [TableName_TestNumber] ON [TableName_PassFailLog].[SerialNumber] = [TableName_TestNumber].[SerialNumber]
WHERE
	(strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestNumber].[Time])-0.2))
	AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestNumber].[Time])+0.2));

DROP VIEW IF EXISTS [vTestValLog];
CREATE VIEW [vTestValLog] AS
SELECT DISTINCT
	[TableName_PassFailLog].[Date],
	[TableName_PassFailLog].[Time],
	[TableName_PassFailLog].[TestTime],
	[TableName_PassFailLog].[NestNum],
	[TableName_PassFailLog].[SerialNumber],
	[TableName_PassFailLog].[SequencePassFail],
	[TableName_PassFailLog].[FailedTests],
	-- [Replace Here]	>2>"\t[TableName_TestVal].["+Locals.CurrentElement+"],\n"
FROM [TableName_PassFailLog]
INNER JOIN [TableName_TestVal] ON [TableName_PassFailLog].[SerialNumber] = [TableName_TestVal].[SerialNumber]
WHERE
	(strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestVal].[Time])-0.2))
	AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestVal].[Time])+0.2));

DROP VIEW IF EXISTS [vTestTimeLog];
CREATE VIEW [vTestTimeLog] AS
SELECT DISTINCT
	[TableName_PassFailLog].[Date],
	[TableName_PassFailLog].[Time],
	[TableName_PassFailLog].[TestTime],
	[TableName_PassFailLog].[NestNum],
	[TableName_PassFailLog].[SerialNumber],
	[TableName_PassFailLog].[SequencePassFail],
	[TableName_PassFailLog].[FailedTests],
	-- [Replace Here]	>2>"\t[TableName_TestTime].["+Locals.CurrentElement+"],\n"
FROM [TableName_PassFailLog]
INNER JOIN [TableName_TestTime] ON [TableName_PassFailLog].[SerialNumber] = [TableName_TestTime].[SerialNumber]
WHERE
	(strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestTime].[Time])-0.2))
	AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestTime].[Time])+0.2));

DROP VIEW IF EXISTS [vTestLimMinLog];
CREATE VIEW [vTestLimMinLog] AS
SELECT DISTINCT
	[TableName_PassFailLog].[Date],
	[TableName_PassFailLog].[Time],
	[TableName_PassFailLog].[TestTime],
	[TableName_PassFailLog].[NestNum],
	[TableName_PassFailLog].[SerialNumber],
	[TableName_PassFailLog].[SequencePassFail],
	[TableName_PassFailLog].[FailedTests],
	-- [Replace Here]	>2>"\t[TableName_TestLimMin].["+Locals.CurrentElement+"],\n"
FROM [TableName_PassFailLog]
INNER JOIN [TableName_TestLimMin] ON [TableName_PassFailLog].[SerialNumber] = [TableName_TestLimMin].[SerialNumber]
WHERE
	(strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestLimMin].[Time])-0.2))
	AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestLimMin].[Time])+0.2));

DROP VIEW IF EXISTS [vTestLimMaxLog];
CREATE VIEW [vTestLimMaxLog] AS
SELECT DISTINCT
	[TableName_PassFailLog].[Date],
	[TableName_PassFailLog].[Time],
	[TableName_PassFailLog].[TestTime],
	[TableName_PassFailLog].[NestNum],
	[TableName_PassFailLog].[SerialNumber],
	[TableName_PassFailLog].[SequencePassFail],
	[TableName_PassFailLog].[FailedTests],
	-- [Replace Here]	>2>"\t[TableName_TestLimMax].["+Locals.CurrentElement+"],\n"
FROM [TableName_PassFailLog]
INNER JOIN [TableName_TestLimMax] ON [TableName_PassFailLog].[SerialNumber] = [TableName_TestLimMax].[SerialNumber]
WHERE
	(strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestLimMax].[Time])-0.2))
	AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestLimMax].[Time])+0.2));

DROP VIEW IF EXISTS [vTestAddtionalNotesLog];
CREATE VIEW [vTestAddtionalNotesLog] AS
SELECT DISTINCT
	[TableName_PassFailLog].[Date],
	[TableName_PassFailLog].[Time],
	[TableName_PassFailLog].[TestTime],
	[TableName_PassFailLog].[NestNum],
	[TableName_PassFailLog].[SerialNumber],
	[TableName_PassFailLog].[SequencePassFail],
	[TableName_PassFailLog].[FailedTests],
	-- [Replace Here]	>2>"\t[TableName_TestAdditionalNotes].["+Locals.CurrentElement+"],\n"
FROM [TableName_PassFailLog]
INNER JOIN [TableName_TestAdditionalNotes] ON [TableName_PassFailLog].[SerialNumber] = [TableName_TestAdditionalNotes].[SerialNumber]
WHERE
	(strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestAdditionalNotes].[Time])-0.2))
	AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestAdditionalNotes].[Time])+0.2));

DROP VIEW IF EXISTS [vYield];
CREATE VIEW [vYield] AS
SELECT
	'Sequnece' AS Test,
	0 AS TestNum,
	s2.*,
	(s2.NumPass + s2.NumFail) AS [Total],
	s2.NumPass*100.0/(s2.NumPass + s2.NumFail)  AS [Yield %]
FROM
(
	SELECT
		COUNT([SequencePassFail]) AS NumPass,
		s.*
	FROM
	(
		SELECT
			COUNT([SequencePassFail]) AS NumFail
		FROM
			[TableName_PassFailLog]
		WHERE [SequencePassFail] = 'FAIL'
	) AS s
	INNER JOIN [TableName_PassFailLog]
	WHERE [SequencePassFail] = 'PASS'
) AS s2
-- [Replace Here]	>0>"\nUNION\nSELECT\n\t'"+Locals.CurrentElement+"',\n\ts2.*,\n\ts2.NumPass + s2.NumFail,\n\ts2.NumPass*100.0/(s2.NumPass + s2.NumFail)\nFROM\n(\n\tSELECT DISTINCT\n\t\t[TableName_TestNumber].["+Locals.CurrentElement+"]+0,\n\t\tCOUNT([TableName_PassFailLog].["+Locals.CurrentElement+"]) AS NumPass,\n\t\ts.*\n\tFROM\n\t(\n\t\tSELECT\n\t\t\tCOUNT(["+Locals.CurrentElement+"]) AS NumFail\n\t\tFROM\n\t\t\t[TableName_PassFailLog]\n\t\tWHERE ["+Locals.CurrentElement+"] = 'FAIL'\n\t) AS s\n\tINNER JOIN [TableName_PassFailLog]\n\tINNER JOIN [TableName_TestNumber] ON [TableName_TestNumber].[SerialNumber] = [TableName_PassFailLog].[SerialNumber]\n\tWHERE\n\t\t[TableName_PassFailLog].["+Locals.CurrentElement+"] = 'PASS'\n\t\tAND strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= strftime('%d%H%M%f',[TableName_TestNumber].[Time])+0.2\n\t\tAND strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= strftime('%d%H%M%f',[TableName_TestNumber].[Time])-0.2\n) AS s2"
ORDER BY TestNum ASC;

DROP VIEW IF EXISTS [vTestTimeStats];
CREATE VIEW [vTestTimeStats] AS
SELECT
	'Sequnece' AS Test,
	0 AS TestNum,
	s.Median,
	AVG(TestTime) AS AverageTime,
	MAX(TestTime) AS MaxTime,
	MIN(TestTime) AS MinTime
FROM
(
	SELECT [TestTime] AS Median FROM [TableName_PassFailLog] ORDER BY [TestTime] LIMIT 1 OFFSET (SELECT COUNT(*) FROM [TableName_PassFailLog])/2
) AS s
INNER JOIN [TableName_PassFailLog]
-- [Replace Here]	>0>"\nUNION\nSELECT DISTINCT\n\t'"+Locals.CurrentElement+"',\n\t[TableName_TestNumber].["+Locals.CurrentElement+"]+0,\n\ts.Median,\n\tAVG([TableName_TestTime].["+Locals.CurrentElement+"]),\n\tMAX([TableName_TestTime].["+Locals.CurrentElement+"]),\n\tMIN([TableName_TestTime].["+Locals.CurrentElement+"])\nFROM\n(\n\tSELECT ["+Locals.CurrentElement+"] AS Median FROM [TableName_TestTime] ORDER BY ["+Locals.CurrentElement+"] LIMIT 1 OFFSET (SELECT COUNT(*) FROM [TableName_TestTime])/2\n) AS s\nINNER JOIN [TableName_TestTime]\nINNER JOIN [TableName_TestNumber] ON [TableName_TestNumber].[SerialNumber] = [TableName_TestTime].[SerialNumber]\nWHERE\n\tstrftime('%d%H%M%f',[TableName_TestTime].[Time])+0.0 <= strftime('%d%H%M%f',[TableName_TestNumber].[Time])+0.2\n\tAND strftime('%d%H%M%f',[TableName_TestTime].[Time])+0.0 >= strftime('%d%H%M%f',[TableName_TestNumber].[Time])-0.2"
ORDER BY TestNum ASC;

DROP VIEW IF EXISTS [vUnionResults];
CREATE VIEW [vUnionResults] AS
SELECT 
	s.[Date],
	s.[Time],
	[TableName_PassFailLog].[TestTime],
	[TableName_PassFailLog].[NestNum],
	[TableName_PassFailLog].[SerialNumber],
	[TableName_PassFailLog].[SequencePassFail],
	[TableName_PassFailLog].[FailedTests],
	-- [Replace Here]	>2>"\ts.["+Locals.CurrentElement+"],\n"
FROM
(
	SELECT
		[SerialNumber],
		[Date],
		[Time],
		-- [Replace Here]	>2>"\t\t["+Locals.CurrentElement+"],\n"
	FROM [TableName_PassFailLog]
	UNION
	SELECT * FROM [TableName_TestNumber]
	UNION
	SELECT * FROM [TableName_TestVal]
	UNION
	SELECT * FROM [TableName_TestTime]
	UNION
	SELECT * FROM [TableName_TestLimMin]
	UNION
	SELECT * FROM [TableName_TestLimMax]
	UNION
	SELECT * FROM [TableName_TestAdditionalNotes]
) AS s
INNER JOIN [TableName_PassFailLog] ON [TableName_PassFailLog].[SerialNumber] = s.[SerialNumber];

DROP VIEW IF EXISTS [vFailedTestDetails];
CREATE VIEW [vFailedTestDetails] AS
SELECT * FROM
(
	WITH SPLIT(Failed, str) AS (
		SELECT '', [TableName_PassFailLog].[FailedTests] FROM [TableName_PassFailLog] WHERE [FailedTests] <> ''
		UNION ALL
			SELECT
				substr(str, 0, instr(str, ',')),
				substr(str, instr(str, ',')+1)
				FROM SPLIT WHERE str!=''
	)
	SELECT DISTINCT
		[TableName_PassFailLog].[Date],
		[TableName_PassFailLog].[Time],
		[TableName_PassFailLog].[TestTime],
		[TableName_PassFailLog].[NestNum],
		[TableName_PassFailLog].[SerialNumber],
		[TableName_PassFailLog].[FailedTests],
		Failed,
		CASE
			-- [Replace Here]	>0>"\t\t\tWHEN Failed='"+Locals.CurrentElement+"' THEN [TableName_TestNumber].["+Locals.CurrentElement+"]\n"
		END AS FailedTestNumber,
		CASE
			-- [Replace Here]	>0>"\t\t\tWHEN Failed='"+Locals.CurrentElement+"' THEN [TableName_TestVal].["+Locals.CurrentElement+"]\n"
		END AS FailedTestVal,
		CASE
			-- [Replace Here]	>0>"\t\t\tWHEN Failed='"+Locals.CurrentElement+"' THEN [TableName_TestTime].["+Locals.CurrentElement+"]\n"
		END AS FailedTestTime,
		CASE
			-- [Replace Here]	>0>"\t\t\tWHEN Failed='"+Locals.CurrentElement+"' THEN [TableName_TestLimMin].["+Locals.CurrentElement+"]\n"
		END AS FailedTestLimMin,
		CASE
			-- [Replace Here]	>0>"\t\t\tWHEN Failed='"+Locals.CurrentElement+"' THEN [TableName_TestLimMax].["+Locals.CurrentElement+"]\n"
		END AS FailedTestLimMax,
		CASE
			-- [Replace Here]	>0>"\t\t\tWHEN Failed='"+Locals.CurrentElement+"' THEN [TableName_TestAdditionalNotes].["+Locals.CurrentElement+"]\n"
		END AS FailedTestAdditionalNotes
	FROM SPLIT
	INNER JOIN [TableName_PassFailLog] ON [TableName_PassFailLog].[FailedTests] LIKE '%'||Failed||'%'
	INNER JOIN [TableName_TestNumber] ON [TableName_TestNumber].[SerialNumber] = [TableName_PassFailLog].[SerialNumber]
	INNER JOIN [TableName_TestVal] ON [TableName_TestVal].[SerialNumber] = [TableName_PassFailLog].[SerialNumber]
	INNER JOIN [TableName_TestTime] ON [TableName_TestTime].[SerialNumber] = [TableName_PassFailLog].[SerialNumber]
	INNER JOIN [TableName_TestLimMin] ON [TableName_TestLimMin].[SerialNumber] = [TableName_PassFailLog].[SerialNumber]
	INNER JOIN [TableName_TestLimMax] ON [TableName_TestLimMax].[SerialNumber] = [TableName_PassFailLog].[SerialNumber]
	INNER JOIN [TableName_TestAdditionalNotes] ON [TableName_TestAdditionalNotes].[SerialNumber] = [TableName_PassFailLog].[SerialNumber]
	WHERE
		(
			Failed != Failed
			-- [Replace Here]	>0>"\t\t\tOR Failed = '"+Locals.CurrentElement+"'\n"
		)
		AND
			(strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestNumber].[Time])-0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestNumber].[Time])+0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestVal].[Time])-0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestVal].[Time])+0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestTime].[Time])-0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestTime].[Time])+0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestLimMin].[Time])-0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestLimMin].[Time])+0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestLimMax].[Time])-0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestLimMax].[Time])+0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 >= (strftime('%d%H%M%f',[TableName_TestAdditionalNotes].[Time])-0.2))
			AND (strftime('%d%H%M%f',[TableName_PassFailLog].[Time])+0.0 <= (strftime('%d%H%M%f',[TableName_TestAdditionalNotes].[Time])+0.2))
);

END;