#/usr/share/env python3
#AC AUTH OK
#AC AUTH DELETE INPROG
from __future__ import generators
import mysql.connector
from mysql.connector import Error
from mysql.connector import errorcode
import time
from serial import Serial
import MySQLdb
#It's for NTP
"""import ntplib
from time import strftime
import datetime
 
c = ntplib.NTPClient()
updateNTP = 1
UNTP = time.time()"""
 
data = []
MCU = []
device = '/dev/ttyACM0'
try:
    print ('Try to connect to'), device
    #print ('Itt')
    MCU = Serial(device, 115200)
    MCU.flush()
    print ('Connection Successful')
except:
    print ('Failed to connect to'), device
def ResultIter(cursor, arraysize=1000):
    while True:
        results = cursor.fetchmany(arraysize)
        if not results:
            break
        for result in results:
            yield result
print ('Waiting for data')
while  True:
   # print ('Itt2')
    #working ntp
    """t1 = time.time()
    if t1 - UNTP >= updateNTP:
        response = c.request('time.google.com', version=3, port='ntp', timeout=10)
        T_NTP = strftime("%Y.%m.%d. %H:%M:%S")
        T_NTP_Y = strftime("%Y")
        T_NTP_Y = T_NTP_Y[2:]
        T_NTP_Y = int(T_NTP_Y)
        T_NTP_M = int(strftime("%m"))
        T_NTP_D = int(strftime("%d"))
        T_NTP_H = int(strftime("%H"))
        T_NTP_m = int(strftime("%M"))
        T_NTP_s = int(strftime("%S"))
        print("Current time and date is: ", T_NTP)
        T_NTP_TO_ARDU = [118, 128, 255, 255, 255]
        T_NTP_TO_ARDU_END = [23, 123, 44, 234, 234, 234]
        T_NTP_TO_ARDU.append(T_NTP_Y)
        T_NTP_TO_ARDU.append(T_NTP_M)
        T_NTP_TO_ARDU.append(T_NTP_D)
        T_NTP_TO_ARDU.append(T_NTP_H)
        T_NTP_TO_ARDU.append(T_NTP_m)
        T_NTP_TO_ARDU.append(T_NTP_s)
        T_NTP_TO_ARDU += T_NTP_TO_ARDU_END
        print(T_NTP_TO_ARDU)
        #test = "118"
        MCU.write(T_NTP_TO_ARDU)
        #print(test)
        UNTP = time.time()"""
    if MCU.in_waiting > 0:
        data = MCU.readline().decode('latin-1').split(",")
        print ("Got data form ARDU", data)
        identifier = data[0]
        print ("Identifier is", identifier)
        #Action to DB, Actuly it's the logger script
        if identifier == "148AD219":
            try:
                connection = MySQLdb.connect("localhost", "phpmyadmin", "987654", "test")
                cursor = connection.cursor()
                number_of_rows = cursor.execute("Select * FROM cards")
                print("Number of rows are: ",number_of_rows)
                NUMCRDS = str(number_of_rows)
                print(NUMCRDS)
                Card_Delete_ID = "129812TUZZ22"
                Card_Delete_ID_Data = Card_Delete_ID
                Card_Delete_ID_Data += ","
                Card_Delete_ID_Data += NUMCRDS
                print(Card_Delete_ID_Data)
                MCU.write(Card_Delete_ID_Data.encode())
                cursor.execute("TRUNCATE TABLE cards")
            except mysql.connector.Error as error:
                print ("Failed to connect to cards {}").format(error)
            finally:
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    NUMCRDS = 0
                    #MCU.write("128244558BB".encode())
                    print ("mysql connection is closed, cards cleared")
        if identifier == "1":
            uid = data[1]
            actionat = data[2]
            actiontype = data[3]
            print ("Whole data is: " + data[0] + data[1] + data[2] + data[3])
            print ("Indetifier is: ", identifier)
            print ("UID is: ", uid)
            print ("Action at: ", actionat)
            print ("Actiontype is: ", actiontype)
            #MCU.flush()
            try:
                connection = mysql.connector.connect(
                host='localhost', database='test',
                user='phpmyadmin', password='987654')
                cursor = connection.cursor()
                mysql_insert_query = ("INSERT INTO unotest"
                "(identifier, uid, actionat, actiontype)"
                "VALUES (%s, %s, %s, %s)")
                recordTuple = (identifier, uid, actionat, actiontype)
                cursor.execute(mysql_insert_query, recordTuple)
                connection.commit()
                print(cursor.rowcount, "Record inserted successfully into unotest table")
                if actiontype ==  "118":
                    returnbyte = "118"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
                elif actiontype == "119":
                    returnbyte = "119"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
                elif actiontype == "18":
                    returnbyte = "18"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
                elif actiontype == "19":
                    returnbyte = "19"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
                elif actiontype == "20":
                    returnbyte = "20"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
                elif actiontype == "21":
                    returnbyte = "21"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
                elif actiontype == "22":
                    returnbyte = "22"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
                elif actiontype == "23":
                    returnbyte = "23"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
                elif actiontype == "24":
                    returnbyte = "24"
                    MCU.write(returnbyte.encode())
                    print("Return byte has send back: ",returnbyte)
            except mysql.connector.Error as error:
                print ("Failed to insert record into unotest {}").format(error)
            finally:
                returnbyte = ""
                if (connection.is_connected()):
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    uid = ""
                    actionat = ""
                    actiontype = ""
                print ("mysql connection is closed")
        #Card Data to DB
        if identifier == "2":
            firstname = data[1]
            lastname = data[2]
            uid = data[3]
            print ("Whole data is: " + data[0] + data[1] + data[2] + data[3])
            print ("Firstname is: ", firstname)
            print ("Lastname is: ", lastname)
            print ("UID is: ", uid)
            #MCU.flush()
            try:
                connection = mysql.connector.connect(
                host='localhost', database='test',
                user='phpmyadmin', password='987654')
                cursor = connection.cursor()
                #check if the UID is already registered
                SEARCHING_UID = (uid,)
                UIDSEARCH = "SELECT uid FROM cards WHERE uid = %s"
                cursor.execute(UIDSEARCH, SEARCHING_UID)
                UIDSEARCHRESULT = []
                UIDSEARCHRESULT = cursor.fetchall()
                if len(UIDSEARCHRESULT) == 0:
                    mysql_insert_query = ("INSERT INTO cards"
                    "(firstname, lastname, uid)"
                    "VALUES (%s, %s, %s)")
                    recordTuple = (firstname, lastname, uid)
                    cursor.execute(mysql_insert_query, recordTuple)
                    connection.commit()
                    print(cursor.rowcount, "Record inserted successfully into Cards table")
                    AC = "556"
                    MCU.write(AC.encode('latin-1'))
                #
                elif len(UIDSEARCHRESULT) != 0:
                    UIDSEARCHRESULTMOD = UIDSEARCHRESULT[0]
                    #str(UIDSEARCHRESULT[0])
                    print("ITT")
                    print ("Removed Data is ", str(UIDSEARCHRESULTMOD[0]))
                    print ("Current UID is", uid)
                    if str(UIDSEARCHRESULTMOD[0]) == uid:
                        print("Card has already in DB")
                        RC = "555"
                        MCU.write(RC.encode('latin-1'))
                    else:
                        mysql_insert_query = ("INSERT INTO cards"
                        "(firstname, lastname, uid)"
                        "VALUES (%s, %s, %s)")
                        recordTuple = (firstname, lastname, uid)
                        cursor.execute(mysql_insert_query, recordTuple)
                        connection.commit()
                        print(cursor.rowcount, "Record inserted successfully into unotest table")
                        AC = "556"
                        MCU.write(AC.encode('latin-1'))
            except mysql.connector.Error as error:
                print ("Failed to insert record into unotest {}").format(error)
            finally:
                if (connection.is_connected()):
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    firstname = ""
                    lastname = ""
                    uid = ""
                    print ("mysql connection is closed")
        #Get Number of Cards From DB
        if identifier == "123":
            print("Try to get the Number of Cards")
            #MCU.flush()
            try:
                connection = MySQLdb.connect("localhost", "phpmyadmin", "987654", "test")
                cursor = connection.cursor()
                number_of_rows = cursor.execute("Select * FROM cards")
                print("Number of rows are: ",number_of_rows)
                NUMCRDS = str(number_of_rows)
                #print(NUMCRDS)
                MCU.write(NUMCRDS.encode('latin-1'))
            except mysql.connector.Error as error:
                print ("Failed to query from unotest {}").format(error)
            finally:
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    NUMCRDS = 0
                    print ("mysql connection is closed")
        #Search Card in DB, then send back "Passed" or "Rejected"
        if identifier == "3":
            uid = data[1]
            try:
                connection = MySQLdb.connect("localhost", "phpmyadmin", "987654", "test")
                cursor = connection.cursor()
                number_of_rows = cursor.execute("Select * FROM cards")
                print("Number of rows are: ",number_of_rows)
                NUMCRDS = str(number_of_rows)
                print("NUMVRDS Contains: ", NUMCRDS)
                if int(NUMCRDS) == 0:
                    print("No Cards in DB")
                    RC = "553"
                    MCU.write(RC.encode('latin-1'))
                elif int(NUMCRDS) != 0:
                    print (uid)
                    connection = mysql.connector.connect(
                    host='localhost', database='test',
                    user='phpmyadmin', password='987654')
                    cursor = connection.cursor()
                    #check if the UID is already registered
                    SEARCHING_UID = (uid,)
                    UIDSEARCH = "SELECT uid FROM cards WHERE uid = %s"
                    cursor.execute(UIDSEARCH, SEARCHING_UID)
                    UIDSEARCHRESULT = []
                    UIDSEARCHRESULT = cursor.fetchall()
                    print (UIDSEARCHRESULT)
                    if len(UIDSEARCHRESULT) != 0:
                        UIDSEARCHRESULTMOD = UIDSEARCHRESULT[0]
                        #str(UIDSEARCHRESULT[0])
                        print("ITT")
                        print ("Removed Data is ", str(UIDSEARCHRESULTMOD[0]))
                        print ("Current UID is", uid)
                        if str(UIDSEARCHRESULTMOD[0]) == uid:
                            print("Passed")
                            RC = "554"
                            MCU.write(RC.encode('latin-1'))
                            ud = 0
                        elif str(UIDSEARCHRESULTMOD[0]) != uid:
                            print("Rejected")
                            RC = "552"
                            MCU.write(RC.encode('latin-1'))
                            ud = 0
                    elif len(UIDSEARCHRESULT) == 0:
                            print("Rejected2")
                            RC = "552"
                            MCU.write(RC.encode('latin-1'))
                            ud = 0
            except mysql.connector.Error as error:
                print ("Failed to query from unotest {}").format(error)
            finally:
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    NUMCRDS = 0
                    print ("mysql connection is closed")
        #Send Cards Data UID to Local EEPROM of Controller
        if identifier == "4":
            rowindex = 0
            print ("Fetch Cards UID")
            try:
                connection = MySQLdb.connect("localhost", "phpmyadmin", "987654", "test")
                cursor = connection.cursor()
                number_of_rows = cursor.execute("Select * FROM cards")
                print("Number of rows are: ",number_of_rows)
                NUMCRDS = str(number_of_rows)
                print("NUMCRDS Contains: ", NUMCRDS)
                if int(NUMCRDS) == 0:
                    print("No Cards in DB")
                    RC = "553"
                    MCU.write(RC.encode('latin-1'))
                elif int(NUMCRDS) != 0:
                    print ("Got uid from DB")
                    Start_character = "-"    #Hypen
                    End_Character = "*"      #Asterisk
                    SendNMCRDS = NUMCRDS + End_Character
                    print(SendNMCRDS)
                    MCU.write(SendNMCRDS.encode())
                    time.sleep(1)
                    connection = mysql.connector.connect(
                    host='localhost', database='test',
                    user='phpmyadmin', password='987654')
                    cursor = connection.cursor()
                    #get each uid
                    UIDSEARCH = "SELECT uid FROM cards"
                    cursor.execute(UIDSEARCH)
                    row = cursor.fetchone()
                    i = 0
                    #itt hagytam abba
                    while row is not None:
                        data = row
                        print (rowindex)
                        print (data)
                        MCU.write(data[0].encode())
                        row = cursor.fetchone()
                        rowindex += 1
                    time.sleep(1)
                    EndOfCardDBData = "??@"
                    print (EndOfCardDBData)
                    MCU.write(EndOfCardDBData.encode())
                    #split
                    #ListCardDataUID = str(ClearedData)
                    #print(ListCardDataUID)
                    #MCU.write(str(row).encode('latin-1'))
            except mysql.connector.Error as error:
                print ("Failed to query from unotest {}").format(error)
            finally:
                print(rowindex, "card data sent")
                i = 0
                rowindex = 0
                cursor.close()
                connection.close()
                data = []
                identifier = ""
                NUMCRDS = 0
                print ("mysql connection is closed")
        #Find user by name
        if identifier == "26":
            
            FirstNameSQL = data[1]
            LastNameSQL = data[2]
            AllNameSQL = data[1] + data[2]
            #print("Current Name is:", AllNameSQL)
            #SELECT * FROM `cards` WHERE firstname LIKE('1234') AND lastname LIKE('56789');
            print("Find user by specified name has STARTED!")
            try:
                connection = MySQLdb.connect("localhost", "phpmyadmin", "987654", "test")
                cursor = connection.cursor()
                number_of_rows = cursor.execute("Select * FROM cards")
                #print("Number of rows are: ",number_of_rows)
                NUMCRDS = str(number_of_rows)
                #print("NUMVRDS Contains: ", NUMCRDS)
                if int(NUMCRDS) == 0:
                    print("No Cards in DB")
                    RC = "553"
                    MCU.write(RC.encode('latin-1'))
                elif int(NUMCRDS) != 0:
                    connection = mysql.connector.connect(
                    host='localhost', database='test',
                    user='phpmyadmin', password='987654')
                    cursor = connection.cursor()
                    UQueryFN = (FirstNameSQL,)
                    UQueryLN = (LastNameSQL,)
                    UQ = "SELECT firstname FROM cards WHERE firstname = %s"
                    cursor.execute(UQ, UQueryFN)
                    UserQuery = cursor.fetchmany(100)
                    #print("From DB1: ", UserQuery)
                    if len(UserQuery) != 0:
                        #print("OK")
                        UQ2 = "SELECT lastname FROM cards WHERE lastname = %s"
                        cursor.execute(UQ2, UQueryLN)
                        
                        UserQuery2 = cursor.fetchmany(1000)
                        #print("From DB2: ", UserQuery2)
                        if len(UserQuery2) != 0:
                            print("************************************")
                            print("User |",FirstNameSQL + " " + LastNameSQL + " |" + " Found!")
                            print("************************************")
                            UD = "214"
                            MCU.write(UD.encode('latin-1'))
                            print("data sent", UD)
                        elif len(UserQuery2) == 0:
                            print("No User Found")
                            UD = "212"
                            MCU.write(UD.encode('latin-1'))
                    elif len(UserQuery) == 0:
                        print("No User Found")
                        UD = "212"
                        MCU.write(UD.encode('latin-1'))
            except mysql.connector.Error as error:
                print ("Failed to query from unotest {}").format(error)
            finally:
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    NUMCRDS = 0
                    print ("mysql connection is closed")
        if identifier == "47":
            UpFirstName = data[1]
            UpLastName = data[2]
            Upuid = data[3]
            print ("Whole data is: " + data[0] + data[1] + data[2] + data[3])
            print ("Firstname is: ", UpFirstName)
            print ("Lastname is: ", UpLastName)
            print ("UID is: ", Upuid)
            try:
                connection = mysql.connector.connect(
                host='localhost', database='test',
                user='phpmyadmin', password='987654')
                cursor = connection.cursor()
                #check if the UID is already registered
                SEARCHING_UID = (Upuid,)
                UIDSEARCH = "SELECT uid FROM cards WHERE uid = %s"
                cursor.execute(UIDSEARCH, SEARCHING_UID)
                UIDSEARCHRESULT = []
                UIDSEARCHRESULT = cursor.fetchall()
                if len(UIDSEARCHRESULT) == 0:
                    mysql_update = ("UPDATE cards SET uid = %s WHERE firstname = %s AND lastname = %s")
                    recordTuple = (Upuid, UpFirstName, UpLastName)
                    cursor.execute(mysql_update, recordTuple)
                    connection.commit()
                    print(cursor.rowcount, "Card updated successfully into Cards table")
                    AC = "556"
                    MCU.write(AC.encode('latin-1'))
                #
                elif len(UIDSEARCHRESULT) != 0:
                    UIDSEARCHRESULTMOD = UIDSEARCHRESULT[0]
                    #str(UIDSEARCHRESULT[0])
                    print("ITT")
                    print ("Removed Data is ", str(UIDSEARCHRESULTMOD[0]))
                    print ("Current UID is", Upuid)
                    if str(UIDSEARCHRESULTMOD[0]) == Upuid:
                        print("Card has already in DB")
                        RC = "555"
                        MCU.write(RC.encode('latin-1'))
                    else:
                        mysql_insert_query = ("INSERT INTO cards"
                        "(firstname, lastname, uid)"
                        "VALUES (%s, %s, %s)")
                        recordTuple = (UpFirstName, UpLastName, Upuid)
                        cursor.execute(mysql_insert_query, recordTuple)
                        connection.commit()
                        print(cursor.rowcount, "Record inserted successfully into unotest table")
                        AC = "556"
                        MCU.write(AC.encode('latin-1'))
            except mysql.connector.Error as error:
                print ("Failed to insert record into unotest {}").format(error)
            finally:
                if (connection.is_connected()):
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    firstname = ""
                    lastname = ""
                    uid = ""
                    print ("mysql connection is closed")
        if identifier == "27":
            UserID = data[1]
            print ("Whole data is: " + data[0] + data[1])
            print ("UserID is: ", UserID)
            print("Find user by specified User ID has started!")
            try:
                connection = MySQLdb.connect("localhost", "phpmyadmin", "987654", "test")
                cursor = connection.cursor()
                number_of_rows = cursor.execute("Select * FROM cards")
                NUMCRDS = str(number_of_rows)
                if int(NUMCRDS) == 0:
                    print("No Cards in DB")
                    RC = "553"
                    MCU.write(RC.encode('latin-1'))
                elif int(NUMCRDS) != 0:
                    connection = mysql.connector.connect(
                    host='localhost', database='test',
                    user='phpmyadmin', password='987654')
                    cursor = connection.cursor()
                    UQueryUserID = (UserID,)
                    USerIDQ = "SELECT id, firstname, lastname, uid FROM cards WHERE id = %s"
                    cursor.execute(USerIDQ, UQueryUserID)
                    UserQuery = cursor.fetchmany(100)
                    if len(UserQuery) != 0:
                        print("************************************")
                        print("User ID |", UserID + " |" + " Found!")
                        print("************************************")
                        UD = "528"
                        MCU.write(UD.encode('latin-1'))
                        print("data sent", UD)
                    elif len(UserQuery) == 0:
                        print("No User Found")
                        UD = "529"
                        MCU.write(UD.encode('latin-1'))
            except mysql.connector.Error as error:
                print ("Failed to query from unotest {}").format(error)
            finally:
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    NUMCRDS = 0
                    print ("mysql connection is closed")
        if identifier == "48":
            UppUserid = data[1]
            UppUid = data[2]                
            print ("Whole data is: " + data[0] + " " + data[1] + " " + data[2])
            print ("User ID is: ", UppUserid)
            print ("UID is: ", UppUid)
            try:
                connection = mysql.connector.connect(
                host='localhost', database='test',
                user='phpmyadmin', password='987654')
                cursor = connection.cursor()
                #check if the UID is already registered
                SEARCHING_UID = (UppUid,)
                UIDSEARCH = "SELECT uid FROM cards WHERE uid = %s"
                cursor.execute(UIDSEARCH, SEARCHING_UID)
                UIDSEARCHRESULT = []
                UIDSEARCHRESULT = cursor.fetchall()
                print(UIDSEARCHRESULT)
                #if card has not been used...
                if len(UIDSEARCHRESULT) == 0:
                    
                    mysql_UserID_Card_Update = ("UPDATE cards SET uid = %s WHERE cards.id = %s")
                    recordTuple = (UppUid, UppUserid)
                    cursor.execute(mysql_UserID_Card_Update, recordTuple)
                    connection.commit()
                    #print(cursor.execute(mysql_UserID_Card_Update, recordTuple))
                    print(cursor.rowcount, "Card updated successfully into Cards table")
                    AC = "556"
                    MCU.write(AC.encode('latin-1'))
                elif len(UIDSEARCHRESULT) != 0:
                    UIDSEARCHRESULTMOD = UIDSEARCHRESULT[0]
                    #str(UIDSEARCHRESULT[0])
                    print("ITT")
                    print ("Removed Data is ", str(UIDSEARCHRESULTMOD[0]))
                    print ("Current UID is", UppUid)
                    if str(UIDSEARCHRESULTMOD[0]) == UppUid:
                        print("Card has already in DB")
                        RC = "555"
                        MCU.write(RC.encode('latin-1'))
                    else:
                        mysql_insert_query = ("UPDATE cards SET uid = %s WHERE cards.id = %s")
                        recordTuple = (UppUid, UppUserid)
                        cursor.execute(mysql_insert_query, recordTuple)
                        connection.commit()
                        print(cursor.rowcount, "Record inserted successfully into unotest table")
                        AC = "556"
                        MCU.write(AC.encode('latin-1'))
            except mysql.connector.Error as error:
                print ("Failed to insert record into unotest {}").format(error)
            finally:
                if (connection.is_connected()):
                    cursor.close()
                    connection.close()
                    data = []
                    identifier = ""
                    firstname = ""
                    lastname = ""
                    uid = ""
                    print ("mysql connection is closed")
                    
    elif MCU.in_waiting < 0:
        print("No HW Connection")