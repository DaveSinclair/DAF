<?php

// -----------------------------------------------------------------------------------------------------------
//
// Restful interface to the DAF MYSQL database
//
//   GET REQUEST
//
//   http://..../rest/<database>/<tablename>/<uid>/<uid>
//
//       eg http://..../rest/daf/cluster/ 
//
//       [ { ID: "5", ParentID: "6", Name: "cluster1", Comments: "just a cluster", Model: "bluebox", Attr1: "valueofattr123", Jff: "fred"}, 
//         { ID: "6", ParentID: "6", Name: "clusterthesecond", Comments: "xxxx", Model: "ModelT", Attr1: "white", Jff: "fred"} ]
//
//   http://..../rest/<tablename>/<id>
//
//       eg http://..../rest/daf/cluster/5 
//
//       [ ( ID: "5", ParentID: "6", Name: "cluster1", Comments: "just a cluster", Model: "bluebox", Attr1: "valueofattr123", Jff: "fred"} ]
//
//   http://..../rest/daf/<tablename>?fieldname=value....
//
//       eg http://..../rest/cluster/?Model=bluebox&ParentID=6
//
//       [ { ID: "5", ParentID: "6", Name: "cluster1", Comments: "just a cluster", Model: "bluebox", Attr1: "valueofattr123", Jff: "fred"}]
//
//
//
//   PUT REQUEST
//
//   POST REQUEST
//
//   DELETE REQUEST
//

require_once('mysql.php');
require_once('json.php');

class RestfulSQL {
    
    var $db_hostname;
    var $db_username;
    var $db_password;
    var $db_database;
    var $db;                                 // connection to the database
    var $method = 'GET';                     // HTTP request method used, eg GET, HEAD, POST, PUT, DELETE 
    var $requestData = NULL;                 // HTTP request data that is sent from the client to this server
    var $tablename = NULL;                   // the database table that is being queried 
    var $uid = NULL;                         // primary key of the database row to query
    var $output = array();                   // an array of strings that is converted into the HTTP response
    var $display = NULL;                     // type of display: database, table or row
    var $baseURL = '/daf/rest';              // baseURL = "/daf/rest"
    var $foreignKeyPostfix = "_uid";
    var $range_start = NULL;                 // Start value in range header - used in GETs
    var $range_number_of_rows = NULL; 
    var $full_number_of_rows  = 0;
    var $sortfield = NULL; 
   
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: RestfulSQL
    //
    // Constructor function for RestfulSQL class. Collects any request data, records the HTTP methods, parses the request URL and determines from that 
    // the requested table name and primary key values
    // expect to enter this routine with 
    //           $_SERVER['REQUEST_URI']     - The URI which was given in order to access this page; for instance:
    //                                         /daf/rest/daf/cluster                 the cluster table
    //                                         /daf/rest/daf/cluster/1               the cluster in the table with ID=1
    //           $_SERVER['REQUEST_METHOD']  - Which request method was used to access the page; i.e. 'GET', 'HEAD', 'POST', 'PUT'.
    //           $_SERVER['CONTENT_LENGTH']  - applies to POSTS 
    //           $_SERVER['HTTP_RANGE']      - of the form 'items=0-24'
    //           $_SERVER['QUERY_STRING']    - the query string  eg if called with /daf/rest/daf/cluster/?Model=bluebox  then $_SERVER['QUERY_STRING']
    //                                         is Model=bluebox
    // ---------------------------------------------------------------------------------------------------------------------------------------
    
    function __construct($db_hostname, $db_username, $db_password, $db_database) {
    
        $this->db_hostname = $db_hostname;
        $this->db_username = $db_username;
        $this->db_password = $db_password;
        $this->db_database = $db_database;
        
        $this->db_username = "root";   $this->db_password = "passw0rd";   /* <<<<<<<<<<< */
    
        if (isset($_SERVER['HTTP_RANGE'])) {
            /* range will be of the form 'items=0-24' */
            $matches = array();
            $num_matches = preg_match('/items=(\d+)-(\d+)/', $_SERVER['HTTP_RANGE'], $matches);
            if ($num_matches == 1) {
               $this->range_start            = $matches[1];
               $this->range_number_of_rows   = $matches[2] - $matches[1] + 1;
            }
        }

        if (isset($_SERVER['REQUEST_URI']) && isset($_SERVER['REQUEST_METHOD'])) {
        
            // collect the reqest data if say this request is a POST and save in $requestData
            if (isset($_SERVER['CONTENT_LENGTH']) && $_SERVER['CONTENT_LENGTH'] > 0) {
                $this->requestData = '';
                $httpContent = fopen('php://input', 'r');
                while ($data = fread($httpContent, 1024)) {
                    $this->requestData .= $data;
                }
                fclose($httpContent);
            }
            
            // find out if there is a query in the REQUEST_URI
            // [REQUEST_URI] will be in the form: /daf/rest/databasename/tablename/uid?param1=val1&param2=val2  or
            //                                    
            //   /daf/rest      is the baseURL
            //   databasename   is the database to be queried,  eg daf for the main DAF database or projects1_phase1_results for a project/phase results database
            //   tablename      is the table in the database that is being queried
            //   uid            is the optional primary key
            //
            //   examples:      /daf/rest/daf/teststand/    for the teststand table
            //                  /daf/rest/blackbird_dvt_results/scenarioresult/       for the scenario result database for project blackbird and phase dvt
            //                  /daf/rest/blackbird_dvt_results/scenarioresult/?sort(+project)   sorting on the name field
            //     
            
            $x = explode("?", $_SERVER['REQUEST_URI']);
            $uristring = $x[0];
            if (isset($x[1])) {
               $querystring = $x[1];
            } else {
               $querystring = "";
            }            
            
            $urlString = substr($uristring, strlen($this->baseURL));
   	        $urlParts = explode('/', $urlString);
			
	        if (isset($urlParts[0]) && $urlParts[0] == '') {   // strip effect of leading '/'
               array_shift($urlParts);
	        }
	        
	        if (isset($urlParts[0])) {
               $this->db_database = $urlParts[0];  // save database if it exists
            }  
			
            if (isset($urlParts[1])) {
               $this->tablename = $urlParts[1];    // save tablename if it exists
            }  

	        // extract primary key(s) from the URL
             
            if (count($urlParts) > 2 && $urlParts[2] != '') {
                array_shift($urlParts);  array_shift($urlParts);      // get rid of the database/tablename and treat rest as primary keys
                foreach ($urlParts as $uid) {
                   if ($uid != '') {
                       $this->uid[] = $uid;
                   }
                }
             }
            
             $this->method = $_SERVER['REQUEST_METHOD'];             // eg GET, POST, PUT etc
             
             // If a sort was specified, find out what field we have to sort on
             // expect query string to be of the form  (+name) or (-name)
             
             if ($querystring != "") {
                $num_matches = preg_match("/sort\(([+-])(.*)\)/", $querystring, $matches);
                if ($num_matches == 1) {
                   if ($matches[1] == "-") {
                      $this->sortfield = $matches[2] . " DESC";
                   } else {
                      $this->sortfield = $matches[2];
                   }
                }
             }
            
         }       
    }
 
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: connect
    //
    // Connnect to the MYSQL database  ??? <<<< do we use this routine?
    //
    // ---------------------------------------------------------------------------------------------------------------------------------------
   
    function connect() {
    
        $this->db = new mysql($this->db_hostname, $this->db_username, $this->db_password); 
        
        if (!$this->db->connect($this->db_database)) {
             trigger_error("Could not connect to server xxx $this->db_hostname, $this->db_username, $this->db_password", E_USER_ERROR);
        }
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: exec
    //
    // Execute the request 
    //
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function exec() {
    
    exec("echo \"$this->db_database   $this->tablename  $this->method\" >/tmp/debug.log");      /* <<<<<<<<<< */
       
        $this->connect();
        
        switch ($this->method) {
            case 'GET':
                $this->get();
                break;
            case 'POST':
                $this->post();
                break;
            case 'PUT':
                $this->put();
                break;
            case 'DELETE':
                $this->delete();
                break;
        }
   
        $this->db->close();
        
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: getPrimaryKeys
    //
    // Get the primary keys for the request table 
    //
    // Returns:   (string) - The primary key field names
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function getPrimaryKeys() {
    
    	return $this->db->getPrimaryKeys($this->tablename);

    }

    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: get
    //
    // Execute a GET request. A GET request fetches a list of tables when no table name is given, a list of rows when a table name is given, 
    // or a table row when a table and primary key(s) are given. It does not change the database contents
    //
    //  A GET request on URL   /daf/rest/            returns a list of all the DAF tables  , eg:
    //
    //                         /daf/rest/daf/cluster    returns a list of the rows in the cluster table as fieldname:value pairs, eg:
    // [ { ID: "5", ParentID: "6", Name: "cluster1", Comments: "just a cluster", Model: "bluebox", Attr1: "valueofattr123", Jff: "fred"}, 
    //   { ID: "6", ParentID: "6", Name: "clusterthesecond", Comments: "xxxx", Model: "ModelT", Attr1: "white", Jff: "fred"} ]
    //
    //                         /daf/rest/daf/cluster/5  returns a list of the fieldname:value pairs for cluster with ID=5, eg:    
    //                                                 [ { ID: "5", ParentID: "6", Name: "cluster1", Comments: "just a cluster"} ]
    //
    //
    // Returns:  
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function get() {
        if ($this->tablename) {
            $primary = $this->getPrimaryKeys();   /* the primary key for all DAF tables will be the ID column */
            if ($primary) {
                //  was a UID specified, eg /rest/cluster/5  - if so we should just get a single row from the table
                if ($this->uid && count($primary) == count($this->uid)) { // get a row
                    $this->display = 'row';
                    $where = '';
                    foreach($primary as $key => $pri) {
                       $where .= $pri.' = \''.$this->uid[$key].'\' AND ';
                    }
                    $where = substr($where, 0, -5);
                    $resource = $this->db->getRows($this->tablename, $where);
                    if ($resource) {
                        if ($this->db->numRows($resource) > 0) {
                            while ($row = $this->db->row($resource)) {
                                $values = array();
                                foreach ($row as $column => $data) {
                                    $field = array(
                                        'field' => $column,
                                        'value' => $data
                                    );
                                    $values[] = $field;
                                }
                                $this->output['row'] = $values;
                            }
                            $this->generateResponseData();
                        } else {
                            $this->notFound();
                        }
                    } else {
                        $this->unauthorized();
                    }
                } else { // get table
                    $this->display = 'table';
                    $this->full_number_of_rows = $this->db->countRows($this->tablename, NULL);
                    $resource = $this->db->getRows($this->tablename, NULL, $this->range_start, $this->range_number_of_rows, $this->sortfield);   // SELECT * FROM tablename LIMIT start, number
       exec("echo \"$this->db_database   $this->tablename  $this->method   range/numnber $this->range_start  $this->range_number_of_rows $this->full_number_of_rows $this->sortfield \" >>/tmp/debug.log");      /* <<<<<<<<<< */
                   if ($resource) {
        exec("echo \"good resource  numrows " . $this->db->numRows($resource) . "  \" >>/tmp/debug.log");      /* <<<<<<<<<< */
                       if ($this->db->numRows($resource) > 0) {
                            while ($row = $this->db->row($resource)) {
                                $values = array();
                                foreach ($row as $column => $data) {
                                    $field = array(
                                        'field' => $column,
                                        'value' => $data
                                    );
                                    $values[] = $field;
                                }
                                $this->output['rows'][] = $values;
                            }
                            $this->generateResponseData();
                        } else {
                            $this->notFound();
                        }
                    } else {
                        $this->unauthorized();  /* <<<<<<<< ???? */
                    }
                }
            }
        } else { // get database
            $this->display = 'database';
            $resource = $this->db->getDatabase();
            if ($resource) {
                if ($this->db->numRows($resource) > 0) {
                    while ($row = $this->db->row($resource)) {
                        $this->output['database'][] = array(
                            'value' => reset($row)
                        );
                    }
                    $this->generateResponseData();
                } else {
                    $this->notFound();
                }
            } else {
                $this->unauthorized();
            }
        }
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: put
    //
    // Execute a PUT request.  
    //
    // Returns:  
    // ---------------------------------------------------------------------------------------------------------------------------------------
    
    function put() {
        if ($this->tablename && $this->uid) {
            if ($this->requestData) {

                $primary = $this->getPrimaryKeys();
                if ($primary && count($primary) == count($this->uid)) { // update a row
                    $pairs = $this->parseRequestData();
                    $values = '';
                    foreach ($pairs as $column => $data) {
                        $values .= '`'.$column.'` = "'.$this->db->escape($data).'", ';
                    }
                    $values = substr($values, 0, -2);
                    $where = '';
                    foreach($primary as $key => $pri) {
                        $where .= $pri . ' = \'' . $this->uid[$key] . '\' AND ';
                    }
                    $where = substr($where, 0, -5);
                    $resource = $this->db->updateRow($this->tablename, $values, $where);
                    if ($resource) {
                        if ($this->db->numAffected() > 0) {
                            $values = array();
                            foreach ($pairs as $column => $data) {
                                $field = array(
                                    'field' => $column,
                                    'value' => $data
                                );
                                if (substr($column, -strlen($this->foreignKeyPostfix)) == $this->foreignKeyPostfix) {
                                }
                                $values[] = $field;
                            }
                            $this->output['row'] = $values;
                            $this->generateResponseData();
                        } else {
                            $this->badRequest();
                        }
                    } else {
                        $this->internalServerError();
                    }
                } else {
                    $this->badRequest();
                }
            } else {
                $this->lengthRequired();
            }
        } elseif ($this->tablename) { // insert a row without a uid
            if ($this->requestData) {
                $pairs = $this->parseRequestData();
                $values = join('", "', $pairs);
                $names = join('`, `', array_keys($pairs));
                $resource = $this->db->insertRow($this->tablename, $names, $values);
                if ($resource) {
                    if ($this->db->numAffected() > 0) {
			$this->created($this->baseURL . '/' . $this->tablename . '/' . $this->db->lastInsertId() . '/');
                    } else {
                        $this->badRequest();
                    }
                } else {
                    $this->internalServerError();
                }
            } else {
                $this->lengthRequired();
            }
        } else {
            $this->methodNotAllowed('GET, HEAD');
        }
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: post 
    //
    // Execute a POST request.  A POST request adds a new row to a table given a table and name=value pairs in the request body
    //
    // Returns:  
    // ---------------------------------------------------------------------------------------------------------------------------------------
    
    function post() {
        if ($this->tablename && $this->uid) {
            if ($this->requestData) {
                $primary = $this->getPrimaryKeys();
                if ($primary && count($primary) == count($this->uid)) { // (attempt to) insert a row with a uid
                    // prepare data for INSERT
                    $pairs = $this->parseRequestData();
                    $values = join('", "', $this->uid).'", "'.join('", "', $pairs);
                    $names  = join('`, `', $primary).'`, `'.join('`, `', array_keys($pairs));
                    
                    // prepare data for a SELECT (i.e. check whether a
                    // row with the same ID/PKey exists)
                    # TODO: the same code is in many other places in this
                    # script, you should better write a function, then call it 
                    $where = '';
                    foreach($primary as $key => $pri) {
                        $where .= $pri.' = \''.$this->uid[$key].'\' AND ';
                    }
                    $where = substr($where, 0, -5);
                    #print("\nWHERE $where\n"); #DEBUG
                    #die(); #DEBUG

                    # imho calling insertRow is not robust because 
                    # relies on mysql failing silently on INSERT, then check 
                    # if number of affected rows == 0 to know wheter to 
                    # perform an UPDATE instead...  PostgreSQL is stricter
                    # and pg_query issues a Warning (which sounds reasonable).
                    # gd <guidoderosa@gmail.com>
                    #$resource = $this->db->insertRow($this->table, $names, $values);
                    # Do a SELECT (check) instead... 
                    $resource = $this->db->getRows($this->tablename, $where);
                    if ($resource && $this->db->numRows($resource) == 0) {
                        $resource = $this->db->insertRow($this->tablename, $names, $values);
                        $this->created();
                    } else {
                        $values = '';
                        foreach ($pairs as $column => $data) {
                            $values .= '`'.$column.'` = "'.$this->db->escape($data).'", ';
                        }
                        $values = substr($values, 0, -2);

                        # WHERE string ($where) already computed
                        #$where = '';
                        #foreach($primary as $key => $pri) {
                        #    $where .= $pri.' = '.$this->uid[$key].' AND ';
                        #}
                        #$where = substr($where, 0, -5);
                        $resource = $this->db->updateRow($this->tablename, $values, $where);
                        if ($resource) {
                            if ($this->db->numAffected() > 0) {
                                $this->noContent();
                            } else {
                                $this->badRequest();
                            }
                        } else {
                            $this->internalServerError();
                        }
                    }
                } else {
                    $this->badRequest();
                }
            } else {
                $this->lengthRequired();
            }
        } elseif ($this->tablename) {
            $this->methodNotAllowed('GET, HEAD, PUT');
        } else {
            $this->methodNotAllowed('GET, HEAD');
        }
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------
    // Function: post
    //
    // Execute a DELETE request. A DELETE request removes a row from the database given a table and primary key(s).
    //
    // Returns:  
    // ---------------------------------------------------------------------------------------------------------------------------------------
    
    function delete() {
        if ($this->tablename && $this->uid) {
            $primary = $this->getPrimaryKeys();
            if ($primary && count($primary) == count($this->uid)) { // delete a row
                $where = '';
                foreach($primary as $key => $pri) {
                    $where .= $pri.' = \''.$this->uid[$key].'\' AND ';
                }
                $where = substr($where, 0, -5);
                $resource = $this->db->deleteRow($this->tablename, $where);
                if ($resource) {
                    if ($this->db->numAffected() > 0) {
                        $this->noContent();
                    } else {
                        $this->notFound();
                    }
                } else {
                    $this->unauthorized();
                }
            }
        } elseif ($this->tablename) {
            $this->methodNotAllowed('GET, HEAD, PUT');
        } else {
            $this->methodNotAllowed('GET, HEAD');
        }
    }
 
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: parseRequestData
    //
    // Parse the HTTP request data.  This is presented in JSON format
    //
    // Returns:  (string)   an array of name value pairs
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function parseRequestData() {
        $values = json_decode($this->requestData);
        return $values;
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: generateResponseData
    //
    // Generate the HTTP response data.
    //
    // Returns:   
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function generateResponseData() {
	   $renderer = new RestfulSQLRenderer($this);
    }
        
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: created
    //
    // * Send a HTTP 201 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function created($url = FALSE) {
        header('HTTP/1.0 201 Created');
        if ($url) {
            header('Location: '.$url);   
        }
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: noContent
    //
    // * Send a HTTP 204 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function noContent() {
        header('HTTP/1.0 204 No Content');
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: badRequest
    //
    // * Send a HTTP 400 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function badRequest() {
        header('HTTP/1.0 400 Bad Request');
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: unauthorised
    //
    // * Send a HTTP 401 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function unauthorized($realm = 'RestfulSQL') {
        header('WWW-Authenticate: Basic realm="'.$realm.'"');
        header('HTTP/1.0 401 Unauthorized');
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: notFound
    //
    // * Send a HTTP 404 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function notFound() {
        header('HTTP/1.0 404 Not Found');
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: methodNotAllowed
    //
    // * Send a HTTP 405 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function methodNotAllowed($allowed = 'GET, HEAD') {
        header('HTTP/1.0 405 Method Not Allowed');
        header('Allow: '.$allowed);
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: notAcceptable
    //
    // * Send a HTTP 406 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function notAcceptable() {
        header('HTTP/1.0 406 Not Acceptable');
        echo "application/json\n";                 /* <<<<< ??? */
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: lengthRequired
    //
    // * Send a HTTP 411 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function lengthRequired() {
        header('HTTP/1.0 411 Length Required');
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------------
    //
    // Function: internalServerError
    //
    // * Send a HTTP 500 response header.
    // ---------------------------------------------------------------------------------------------------------------------------------------

    function internalServerError() {
        header('HTTP/1.0 500 Internal Server Error');
    }
    
}

?>
