BEGIN TRANSACTION;

		-- [Replace Here]	>0>"ALTER TABLE [TableName_NonTestTimes_Batch] ADD \n\t\t["+Locals.CurrentElement+"] TEXT;\n"

		-- [Replace Here]	>0>"ALTER TABLE [TableName_NonTestTimes_UUT] ADD \n\t\t["+Locals.CurrentElement+"] TEXT;\n"

		-- [Replace Here]	>0>"ALTER TABLE [TableName_NonTestNotes_Batch] ADD \n\t\t["+Locals.CurrentElement+"] TEXT;\n"

		-- [Replace Here]	>0>"ALTER TABLE [TableName_NonTestNotes_UUT] ADD \n\t\t["+Locals.CurrentElement+"] TEXT;\n"

END;