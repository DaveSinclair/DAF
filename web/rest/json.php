<?php

/**
 * PHP REST SQL JSON renderer class
 * This class renders the REST response data as JSON.
 */
class RestfulSQLRenderer {
   
    /**
     * @var restfulSQL PHPRestSQL
     */
    var $restfulSQL;
   
    /**
     * Constructor.
     * @param restfulSQL restfulSQL
     */
    function __construct($restfulSQL) {
        $this->restfulSQL = $restfulSQL;
        switch($restfulSQL->display) {
            case 'database':
                $this->database();
                break;
            case 'table':
                $this->table();
                break;
            case 'tableids':
                $this->tableids();
                break;
            case 'row':
                $this->row();
                break;
        }
    }
    
    /**
     * Output the top level table listing.
     */
    function database() {
        header('Content-Type: application/json');
        echo '{' . "\n";
        if (isset($this->restfulSQL->output['database'])) {
            foreach ($this->restfulSQL->output['database'] as $table) {
                echo ' id: "' . htmlspecialchars($table['value']) . '"' . "\n";
            }
        }
        echo '}' . "\n";
    }
    
    /**
     * Output the ids within a table.
     */
    function tableids() {
        $i = 0;
        header('Content-Type: application/json');
        echo '{' . "\n";
        if (isset($this->restfulSQL->output['table'])) {
            foreach ($this->restfulSQL->output['table'] as $row) {
                if ($i !=0) {
                   echo ", ";
                }
                // ensure there are no funny " in the field that is returned
                // ensure there is no \n in the field that is returned
                echo ' id: "' . str_replace("\n", " ", htmlspecialchars($row['value'])) . '"' . "\n";
                $i++;
            }
        }
        echo '}' . "\n";
    }
    
     /**
     * Output the entry in a number of table rows
     */
    function table() {
        header('Content-Type: application/json');
        header('Content-Range: items ' .  $this->restfulSQL->range_start . '-' . ($this->restfulSQL->range_start + $this->restfulSQL->range_number_of_rows - 1 ) .  
               '/' . $this->restfulSQL->full_number_of_rows ); /* <<<< */
        echo '[ ';
        if (isset($this->restfulSQL->output['rows'])) {
            $i = 0;
            foreach ($this->restfulSQL->output['rows'] as $row) {
                if ($i != 0) {
                   echo ", ";
                }
                echo '{ ';
                $j = 0;
                foreach ($row as $field) {
                   $fieldName = $field['field'];
                   if ($j !=0) {
                      echo ", ";
                   }
                // ensure there are no funny " in the field that is returned
                // ensure there is no \n in the field that is returned
                
                   echo $fieldName . ': "' . str_replace("\n", " ", htmlspecialchars($field['value'])) . '"';
                   $j++;
                }
                $i++;
                echo '}';
            }
        }
        echo ' ]' . "\n";
    }

    /**
     * Output the entry in a table row.
     */
    function row() {
        $i = 0;
        header('Content-Type: application/json');
        echo '[ {' . "\n";
        if (isset($this->restfulSQL->output['row'])) {
            foreach ($this->restfulSQL->output['row'] as $field) {
                $fieldName = $field['field'];
                if ($i !=0) {
                   echo ", ";
                }
                // ensure there are no funny " in the field that is returned
                // ensure there is no \n in the field that is returned
                echo $fieldName . ': "' . str_replace("\n", " ", htmlspecialchars($field['value'])) . '"';
                $i++;
            }
        }
        echo '} ]' . "\n";
    }

}
