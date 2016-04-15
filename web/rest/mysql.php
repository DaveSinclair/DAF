<?php

//---------------------------------------------------------------------------------------------------------------
//
// mysql class
//
// methods:  __construct($db_hostname, $db_username, $db_password)
//           connect($database)
//           close()
//           select_db($database)
//           getColumns($database)
//           getColumns($tablename) 
//           getRows($tablename, $where = NULL, start = NULL, number_of_rows = NULL)
//           countRows($tablename, $where = NULL)
//           getTable($primary, $tablename)
//           getDatabase()
//           getPrimaryKeys($tablename)
//           updateRow($tablename, $values, $where)
//           insertRow($tablename, $names, $values)
//           deleteRow($tablnamee, $where)
//           escape($string)
//           row($resource)
//           numRows($resource)
//           numAffected()
//           lastInsertId()
//
//---------------------------------------------------------------------------------------------------------------

class mysql {

    var $db_hostname;    // the server running the database
    var $db_username;    // the database username
    var $db_password;    // the password for the database username
    var $db;             // the database resource
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   __construct                                                               */
    /*                                                                                       */
    /* Purpose:        class constructor                                                     */
    /*                                                                                       */
    /* Parameters:     db_hostname            the server running the database                */
    /*                 db_username            the database username                          */
    /*                 db_password            the database password                          */
    /*                                                                                       */
    /* Returns         nothing                                                               */    
    /* ------------------------------------------------------------------------------------- */

    function __construct($db_hostname, $db_username, $db_password) {
    
        $this->db_hostname = $db_hostname;
        $this->db_username = $db_username;
        $this->db_password = $db_password;
        
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   connect                                                                   */
    /*                                                                                       */
    /* Purpose:        establish a connection to the database server                         */
    /*                                                                                       */
    /* $database                                                                             */
    /*                                                                                       */
    /* Returns         bool                   TRUE for success, FALSE for failure            */    
    /* ------------------------------------------------------------------------------------- */

    function connect($database) {
    
        if ($this->db = @mysql_pconnect($this->db_hostname, $this->db_username, $this->db_password)) {  /* <<<< should this be a persistent connection ????? */
           if ($this->select_db($database)) {
              return TRUE;
           } else {
              exec("echo \"select_db failed for $database  this->db=$this->db_database   this->tablename=$this->tablename  $this->method\" >>/tmp/debug.log");      /* <<<<<<<<<< */
           }
        }
        return FALSE;
    }

    /* ------------------------------------------------------------------------------------- */
    /* Function:   close                                                                     */
    /*                                                                                       */
    /* Purpose:        close a connection to the database server                             */
    /*                                                                                       */
    /* Parameters:     none                                                                  */
    /*                                                                                       */
    /* Returns         bool                   success (TRUE) or (FAIL)                       */    
    /* ------------------------------------------------------------------------------------- */

    function close() {
        mysql_close($this->db);
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   select_db                                                                 */
    /*                                                                                       */
    /* Purpose:        select a database (on an existing server connection)                  */
    /*                                                                                       */
    /* Parameters:     database               the database name                              */
    /*                                                                                       */
    /* Returns         bool                   success (TRUE) or (FAIL)                       */    
    /* ------------------------------------------------------------------------------------- */

    function select_db($database) {
        if (mysql_select_db($database, $this->db)) {
            return TRUE;
        }
        return FALSE;
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   getColumns                                                                */
    /*                                                                                       */
    /* Purpose:        get the column names in a given table                                 */
    /*                                                                                       */
    /* Parameters:     tablename              the database table name                        */
    /*                                                                                       */
    /* Returns         a resultset resource   the columns in the table                       */    
    /* ------------------------------------------------------------------------------------- */

    function getColumns($tablename) {
        return mysql_query('SHOW COLUMNS FROM ' . $tablename, $this->db);
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   getRows                                                                   */
    /*                                                                                       */
    /* Purpose:        get the field values for 0 or more rows in a table                    */
    /*                                                                                       */
    /* Parameters:     tablename              the database table name                        */
    /*                 where                  a where clause selecting rows from the table   */
    /*                                        may be NULL or omitted if all rows are wanted  */
    /*                 offset                 the row number to start from, if set to NULL   */
    /*                                        or omitted, this defaults to 0                 */
    /*                 number_of_rows         the number of rows to be returned, if set to   */
    /*                                        NULL or omitted, this defaults to the entire   */
    /*                                        table                                          */
    /* Returns         a resultset resource   0 or more rows from the table                  */    
    /* ------------------------------------------------------------------------------------- */

     function getRows($tablename, $where = NULL, $offset = NULL, $number_of_rows = NULL, $sortfield = NULL) {

        $where_clause = '';
        $offset_clause = '';
        $number_of_rows_clause = '';
        $sortfield_clause = '';

        if (isset($where)) {
            $where_clause = "WHERE $where";
        }
        if (isset($offset)) {
            $offset_clause = "OFFSET $offset";
        }
        if (isset($number_of_rows)) {
            $number_of_rows_clause = "LIMIT $number_of_rows";
        }

        if (isset($sortfield)) {
            $sortfield_clause = "ORDER BY $sortfield";
        }
        exec("echo \" sortfield $sortfield sortfieldclause $sortfield_clause \" >>/tmp/debug.log");      /* <<<<<<<<<< */

        $query = sprintf('SELECT * FROM %s %s %s %s %s', $tablename, $where_clause, $sortfield_clause, $number_of_rows_clause, $offset_clause);
        exec("echo \" $query \" >>/tmp/debug.log");      /* <<<<<<<<<< */

        return mysql_query($query);

    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   countRows                                                                 */
    /*                                                                                       */
    /* Purpose:        gets the number of rows in a table for a where clause                 */
    /*                                                                                       */
    /* Parameters:     tablename              the database table name                        */
    /*                 where                  a where clause selecting rows from the table   */
    /*                                        may be NULL or omitted if all rows are wanted  */
    /* Returns         N                      number of rows                                 */    
    /* ------------------------------------------------------------------------------------- */

     function countRows($tablename, $where = NULL) {

        $numrows = 0;
        
        $where_clause = '';

        if (isset($where)) {
            $where_clause = "WHERE $where";
        }

        $query = sprintf('SELECT * FROM %s %s', $tablename, $where_clause);
        $result = mysql_query($query);
        if ($query)  { 
          $numrows = mysql_num_rows($result);
        }
        
        return $numrows;
    }

    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   getTable                                                                  */
    /*                                                                                       */
    /* Purpose:        get the field values for certain fields in all rows of the table      */
    /*                                                                                       */
    /* Parameters:     primary                A comma separated list of column names         */
    /*                 tablename              the database table name                        */
    /* Returns         a resultset resource   0 or more rows from the table                  */    
    /* ------------------------------------------------------------------------------------- */
    function getTable($primary, $tablename) {
        return mysql_query(sprintf('SELECT %s FROM %s', $primary, $tablename));
    }

    /* ------------------------------------------------------------------------------------- */
    /* Function:   getDatabase                                                               */
    /*                                                                                       */
    /* Purpose:        get the output of the SHOW TABLES command                             */
    /*                                                                                       */
    /* Returns         a resultset resource   A list of table names                          */     
    /* ------------------------------------------------------------------------------------- */

    function getDatabase() {
        return mysql_query('SHOW TABLES');
    }

    /* ------------------------------------------------------------------------------------- */
    /* Function:   getPrimaryKeys                                                            */
    /*                                                                                       */
    /* Purpose:        get the field names of the primary keys in a given table              */
    /*                                                                                       */
    /* Parameters:     tablename              the databse table name                         */
    /*                                                                                       */
    /* Returns         a string               the primary key field names for this table     */    
    /* ------------------------------------------------------------------------------------- */

    function getPrimaryKeys($tablename) {
        $resource = $this->getColumns($tablename);
        $primary = NULL;
        if ($resource) {
            while ($row = $this->row($resource)) {
                if ($row['Key'] == 'PRI') {
                    $primary[] = $row['Field'];
                }
            }
        }
        return $primary;
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   updateRow                                                                 */
    /*                                                                                       */
    /* Purpose:        update all field values in 0 or more rows in a given table            */
    /*                                                                                       */
    /* Parameters:     tablename              the database table name                        */
    /*                 values                 the values for the row                         */
    /*                 where                  a where clause selecting rows from the table   */
    /*                                                                                       */
    /* Returns         a bool value           success or failure                             */    
    /* ------------------------------------------------------------------------------------- */

    function updateRow($tablename, $values, $where) {
        return mysql_query(sprintf('UPDATE %s SET %s WHERE %s', $tablename, $values, $where));
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   insertRow                                                                 */
    /*                                                                                       */
    /* Purpose:        Insert a new row                                                      */
    /*                                                                                       */
    /* Parameters:     tablename              the databse table name                         */
    /*                 names                  the fiields for the row                        */
    /*                 values                 the values for the row                         */
    /*                 where                  a where clause selecting rows from the table   */
    /*                                                                                       */
    /* Returns         a bool value           success or failure                             */    
    /* ------------------------------------------------------------------------------------- */

    function insertRow($tablename, $names, $values) {
        return mysql_query(sprintf('INSERT INTO %s (`%s`) VALUES ("%s")', $tablename, $names, $values));
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   deleteRow                                                                 */
    /*                                                                                       */
    /* Purpose:        Delete an existing row                                                */
    /*                                                                                       */
    /* Parameters:     tablename              the database table name                        */
    /*                 where                  a where clause selecting rows from the table   */
    /*                                                                                       */
    /* Returns         a bool value           success or failure                             */    
    /* ------------------------------------------------------------------------------------- */

    function deleteRow($tablename, $where) {
        return mysql_query(sprintf('DELETE FROM %s WHERE %s', $tablename, $where));
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   escape                                                                    */
    /*                                                                                       */
    /* Purpose:        Escape a string to be part of the database query.                     */
    /*                                                                                       */
    /* Parameters:     string                 the string to escape                           */
    /*                                                                                       */
    /* Returns         a string               the escaped string                             */    
    /* ------------------------------------------------------------------------------------- */

    function escape($string) {
        return mysql_escape_string($string);
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   row                                                                       */
    /*                                                                                       */
    /* Purpose:        Fetch a row from a query resultset resource                           */
    /*                                                                                       */
    /* Parameters:     resource               the resultset                                  */
    /*                                                                                       */
    /* Returns         an array               An array of the fields and values from the     */
    /*                                        next row in the resultset                      */
    /* ------------------------------------------------------------------------------------- */

    function row($resource) {
        return mysql_fetch_assoc($resource);
    }

    /* ------------------------------------------------------------------------------------- */
    /* Function:   numRows                                                                   */
    /*                                                                                       */
    /* Purpose:        Determine the number of rows in the resultset                         */
    /*                                                                                       */
    /* Parameters:     resource               the resultset                                  */
    /*                                                                                       */
    /* Returns         int                    the number of rows                             */
    /* ------------------------------------------------------------------------------------- */

    function numRows($resource) {
        return mysql_num_rows($resource);
    }

    /* ------------------------------------------------------------------------------------- */
    /* Function:   numAffected                                                               */
    /*                                                                                       */
    /* Purpose:        Determine the number of rows affected by a query                      */
    /*                                                                                       */
    /* Parameters:     resource               the resultset                                  */
    /*                                                                                       */
    /* Returns         int                    the number of rows                             */
    /* ------------------------------------------------------------------------------------- */

    function numAffected() {
        return mysql_affected_rows($this->db);
    }
    
    /* ------------------------------------------------------------------------------------- */
    /* Function:   lastInsertId                                                              */
    /*                                                                                       */
    /* Purpose:        Determine the ID of the last inserted record                          */
    /*                                                                                       */
    /* Parameters:     none                                                                  */
    /*                                                                                       */
    /* Returns         int                    the number of rows                             */
    /* ------------------------------------------------------------------------------------- */

    function lastInsertId() {
        return mysql_insert_id();
    }
    
}
?>
