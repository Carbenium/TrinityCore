-- NPC Engineer Helice
UPDATE creature_text SET emote = 5 WHERE CreatureID = 28787 AND GroupID = 0;
UPDATE creature_text SET emote = 25 WHERE CreatureID = 28787 AND GroupID = 1;
UPDATE creature_text SET emote = 11 WHERE CreatureID = 28787 AND GroupID = 3;
UPDATE creature_text SET emote = 25 WHERE CreatureID = 28787 AND GroupID = 4;
UPDATE creature_text SET emote = 5 WHERE CreatureID = 28787 AND GroupID = 5;
UPDATE creature_text SET emote = 4 WHERE CreatureID = 28787 AND GroupID = 6;

DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry IN (52369, 52371);
INSERT INTO conditions (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName, Comment) VALUES
(13, 1, 52369, 0, 0, 31, 0, 3, 28789, 0, 0, 0, 0, '', 'Spell ''Detonate Explosives'' targets trigger'),
(13, 1, 52371, 0, 0, 31, 0, 3, 28789, 0, 0, 0, 0, '', 'Spell ''Detonate Explosives'' targets trigger');

DELETE FROM spell_script_names WHERE spell_id IN (52369, 52371);
INSERT INTO spell_script_names (spell_id, ScriptName) VALUES
(52369, 'spell_q12688_detonate_1'),
(52371, 'spell_q12688_detonate_2');
