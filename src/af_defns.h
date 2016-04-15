#ifndef _AF_DEFNS_H
#define _AF_DEFNS_H

#include "daf_protocol.h"

typedef char datetime[DAF_DATETIME_LEN];

#define WORKREQUEST_FIELDS "ID, Project, Phase, RequestType, TesterID, State, Tablename1, ObjectID1, Tablename2, ObjectID2, Tablename3, ObjectID3, Tablename4, ObjectID4, Tablename5, ObjectID5, description1, Description1Type, Logdirectory, Scenariologfilename"
typedef struct workrequest
{
    Iu32      ID;
    char      Project[DAF_PROJECT_LEN];
    char      Phase[DAF_PHASE_LEN];
    char      RequestType[DAF_REQUESTTYPE_LEN];
    int       TesterID;
    char      State[DAF_STATE_LEN];
    char      Tablename1[32];
    Iu32      ObjectID1;
    char      Tablename2[32];
    Iu32      ObjectID2;
    char      Tablename3[32];
    Iu32      ObjectID3;
    char      Tablename4[32];
    Iu32      ObjectID4;
    char      Tablename5[32];
    Iu32      ObjectID5;
    char      Description1[DAF_DESCRIPTOR1_LEN];
    char      Description1Type[DAF_DESCRIPTOR1TYPE_LEN];
    char      Logdirectory[DAF_LOGLOCATION_LEN];
    char      Scenariologfilename[DAF_FILENAME_LEN];
} workrequest_t;

#define WORKQUEUE_FIELDS "ID, WorkrequestID, Project, Phase, ScenarioID, TestlevelID, TeststandID, ScenarioresultID, ActionresultID, EnvironmentID, OutcomeactionID, TesterID, MaillistID, Actiontype, Stepnumber, Hostname, State, Statemodifier, Testcase, Invocation, Duration, Maxduration, Pass, Start, End, Tag, Loglocation, Scenariologfilename"
typedef struct workqueue
{
    Iu32      ID;
    Iu32      WorkrequestID;
    char      Project[DAF_PROJECT_LEN];
    char      Phase[DAF_PHASE_LEN];
    Iu32      ScenarioID;
    Iu32      TestlevelID;
    Iu32      TeststandID;
    Iu32      ScenarioresultID;
    Iu32      ActionresultID;
    Iu32      EnvironmentID;
    Iu32      OutcomeactionID;
    Iu32      TesterID;
    Iu32      MaillistID;
    char      Actiontype[DAF_ACTIONTYPE_LEN];
    Iu32      Stepnumber;
    char      Hostname[64];
    char      State[DAF_STATE_LEN];
    char      Statemodifier[DAF_STATEMODIFIER_LEN];
    char      Testcase[DAF_TESTCASE_LEN];
    char      Invocation[DAF_INVOCATION_LEN];
    char      Duration[DAF_DURATION_LEN];
    Iu32      Maxduration;
    int       Pass;
    datetime  Start;
    datetime  End;
    Iu32      Tag;
    char      Loglocation[DAF_LOGLOCATION_LEN];
    char      Scenariologfilename[DAF_FILENAME_LEN];
} workqueue_t;

#define SCENARIO_FIELDS "ID, Name, Author, OutcomeactionID, Createdon, Lastchanged"
typedef struct scenario
{
    Iu32      ID;
    char      Name[DAF_SCENARIO_LEN];
    char      Author[DAF_AUTHOR_LEN];
    Iu32      OutcomeactionID;
    datetime  Createdon;
    datetime  Lastchanged;
} scenario_t;

#define SCENARIORESULT_FIELDS "ID, Project, Phase, Scenarioname, Jobname, State, Actionsinscenario, Actionsattempted, Actionscompleted, Actionspassed, Actionsfailed, Pass, Start, End, Teststand, TeststandrecordID, Testlevel, TestlevelrecordID, Tester, Loglocation, Scenariologfilename, Comments"
typedef struct scenarioresult
{
    Iu32      ID;
    char      Project[DAF_PROJECT_LEN];
    char      Phase[DAF_PHASE_LEN];
    char      Scenarioname[DAF_SCENARIO_LEN];
    char      Jobname[DAF_JOBNAME_LEN];
    char      State[DAF_STATE_LEN];
    int       Actionsinscenario;
    int       Actionsattempted;
    int       Actionscompleted;
    int       Actionspassed;
    int       Actionsfailed;
    int       Pass;
    datetime  Start;
    datetime  End;
    char      Teststand[DAF_TESTSTAND_LEN];
    int       TeststandrecordID;
    char      Testlevel[DAF_LEVEL_LEN];
    int       TestlevelrecordID;
    char      Tester[DAF_TESTER_LEN];
    char      Loglocation[DAF_LOGLOCATION_LEN];
    char      Scenariologfilename[DAF_FILENAME_LEN];
    char      Comments[DAF_COMMENTS_LEN];
} scenarioresult_t;

#define ACTIONRESULT_FIELDS "ID, ScenarioresultID, ActionType, Stepnumber, Status, Hostname, Testcase, Invocation, Pass, Start, End, Comments"
typedef struct actionresult
{
    Iu32      ID;
    Iu32      ScenarioresultID;
    char      Actiontype[DAF_ACTIONTYPE_LEN];
    Iu32      Stepnumber;
    char      Status[DAF_STATUS_LEN];
    char      Hostname[DAF_HOST_NAME_LEN];
    char      Testcase[DAF_TESTCASE_LEN];
    char      Invocation[DAF_INVOCATION_LEN];
    Iu32      Pass;
    datetime  Start;
    datetime  End;
    char      Comments[DAF_COMMENTS_LEN];
} actionresult_t;

#define ACTION_FIELDS "ID, ScenarioID, ActiontypeID, Description, Stepnumber, Hostselectortype, Hostselectorvalue, CollectorsetID, TestcaseID, Parameters, EnvironmentID, Duration, Maxduration"
typedef struct action
{
    Iu32      ID;
    Iu32      ScenarioID;
    Iu32      ActiontypeID;
    char      Description[64];
    Iu32      Stepnumber;
    char      Hostselectortype[DAF_HOSTSELECTORTYPE_NAME_LEN];
    char      Hostselectorvalue[DAF_HOSTSELECTORVALUE_NAME_LEN];
    Iu32      CollectorsetID;
    Iu32      TestcaseID;
    char      Parameters[DAF_PARAMETERS_LEN];
    Iu32      EnvironmentID;
    char      Duration[DAF_DURATION_LEN];
    Iu32      Maxduration;
} action_t;

#define ACTIONTYPE_FIELDS "ID, Type, Description"
typedef struct actiontype
{
    Iu32      ID;
    char      Type[DAF_ACTIONTYPE_LEN];
    char      Description[DAF_ACTIONDESCRIPTION_LEN];
} actiontype_t;

#define TESTCASE_FIELDS "ID, Name, Description"
typedef struct testcase
{
    Iu32      ID;
    char      Name[DAF_TESTCASE_LEN];
    char      Description[DAF_TESTCASE_DESCRIPTION_LEN];
} testcase_t;

#define PARAMETER_FIELDS "ID, Name, Defaultvalue, Description"
typedef struct parameter
{
    Iu32      ID;
    char      Name[DAF_PARAMETER_NAME_LEN];
    char      Defaultvalue[DAF_PARAMETER_DEFAULTVALUE_LEN];
    char      Description[DAF_PARAMETER_DESCRIPTION_LEN];
} parameter_t;

typedef struct userdefinedparameter
{
    char      Name[DAF_PARAMETER_NAME_LEN];
    char      Value[DAF_PARAMETER_VALUE_LEN];
} userdefinedparameter_t;

#define HOST_FIELDS "ID, TeststandID, Name, Type, Model, Serial, Hostselectorvalue, Teststandprimary, Comments, Agentstatus, Agentstatusdate"
typedef struct host
{
    Iu32      ID;
    Iu32      TeststandID;
    char      Name[DAF_HOST_NAME_LEN];
    char      Type[DAF_HOST_TYPE_LEN];
    char      Model[DAF_HOST_MODEL_LEN];
    char      Serial[DAF_HOST_SERIAL_LEN];
    char      Hostselectorvalue[DAF_HOSTSELECTORVALUE_NAME_LEN];
    int       Teststandprimary;
    char      Comments[DAF_HOST_COMMENTS_LEN];
    char      Agentstatus[DAF_HOST_AGENTSTATUS_LEN];
    datetime  Agentstatusdate;
} host_t;

#define HOSTSELECTORTYPE_FIELDS "ID, Name"
typedef struct hostselectortype
{
    Iu32      ID;
    char      Name[DAF_HOSTSELECTORTYPE_NAME_LEN];
} hostselectortype_t;

#define HOSTSELECTORVALUE_FIELDS "ID, Value"
typedef struct hostselectorvalue
{
    Iu32      ID;
    char      Value[DAF_HOSTSELECTORVALUE_NAME_LEN];
} hostselectorvalue_t;

#define COLLECTORVALUE_FIELDS "ID, CollectortypeID, HostID, Value"
typedef struct collectorvalue
{
    Iu32      ID;
    Iu32      CollectortypeID;
    Iu32      HostID;
    char      Value[DAF_COLLECTORVALUE_LEN];
} collectorvalue_t;

#define COLLECTORTYPE_FIELDS "ID, Name, Builtin, OStype, Invocation"
typedef struct collectortype
{
    Iu32        ID;
    char        Name[DAF_COLLECTORTYPE_NAME_LEN];
    int         Builtin;
    char        OStype[DAF_COLLECTORTYPE_OSTYPE_LEN];
    char        Invocation[DAF_COLLECTORTYPE_INVOCATION_LEN];
} collectortype_t;

#define ENVIRONMENT_FIELDS "ID, Name, Description"
typedef struct environment
{
    Iu32        ID;
    char        Name[DAF_ENVIRONMENT_NAME_LEN];
    char        Description[DAF_ENVIRONMENT_DESCRIPTION_LEN];
} environment_t;

#define ENVIRONMENTVALUE_FIELDS "ID, Name, Value, EnvironmentID"
typedef struct environmentvalue
{
    Iu32        ID;
    char        Name[DAF_ENVIRONMENTVALUE_NAME_LEN];
    char        Value[DAF_ENVIRONMENTVALUE_VALUE_LEN];
    int         EnvironmentID;
} environmentvalue_t;

typedef struct environment_assignment
{
    Iu32        ID;
    char        Value[DAF_ENVIRONMENTVALUE_NAME_LEN+DAF_ENVIRONMENTVALUE_VALUE_LEN + 4];
} environment_assignment_t;

#define COLLECTORSET_FIELDS "ID, Name, Description"
typedef struct collectorset
{
    Iu32        ID;
    char        Name[DAF_COLLECTORSET_NAME_LEN];
    char        Description[DAF_COLLECTORSET_DESCRIPTION_LEN];
} collectorset_t;

#define COLLECTORSETMEMBER_FIELDS "ID, CollectortypeID, Requiredvalue, CollectorsetID"
typedef struct collectorsetmember
{
    Iu32        ID;
    int         CollectortypeID;
    char        Requiredvalue[DAF_COLLECTORVALUE_LEN];
    int         CollectorsetID;
} collectorsetmember_t;

#define OUTCOMEACTION_FIELDS "ID, Name, Actiononpass, Actiononfail, Scenariotoberunonpass, Scenariotoberunonfail, Erroraction, Notificationonpass, NotificationonFail, TesterID, MaillistID"
typedef struct outcomeaction
{
    Iu32        ID;
    char        Name[DAF_OUTCOMEACTION_NAME_LEN];
    char        Actiononpass[DAF_ACTIONONPASS_LEN];
    char        Actiononfail[DAF_ACTIONONFAIL_LEN];
    int         Scenariotoberunonpass;
    int         Scenariotoberunonfail;
    char        Erroraction[DAF_ERRORACTION_LEN];
    char        Notificationonpass[DAF_NOTIFICATIONONPASS_LEN];
    char        Notificationonfail[DAF_NOTIFICATIONONFAIL_LEN];
    int         TesterID;
    int         MaillistID;
} outcomeaction_t;

#define USER_FIELDS "user.ID, user.Firstname, user.Lastname, user.Area, user.Email, user.Admin, user.Password"
typedef struct user
{
    Iu32        ID;
    char        Firstname[DAF_FIRSTNAME_LEN];
    char        Lastname[DAF_LASTNAME_LEN];
    char        Area[DAF_AREA_LEN];
    char        Email[DAF_EMAIL_LEN];
    bool_t      Admin;
    char        Password[DAF_PASSWORD_LEN];
} user_t;

#define OBJECTTYPE_FIELDS "ID, ParentID, Name, Description"
typedef struct objecttype
{
    Iu32        ID;
    Iu32        ParentID;
    char        Name[DAF_OBJECTTYPENAME_LEN];
    char        Description[DAF_OBJECTTYPEDESCRIPTION_LEN];
} objecttype_t;

#endif
