/* 
 * File:   main.cpp
 * Author: palmyman
 *
 * Created on May 7, 2015, 9:30 PM
 */

#ifndef __PROGTEST__
#include <cassert>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <algorithm>
using namespace std;

class CResult {
public:

    CResult(const string & name,
            unsigned int studentID,
            const string & test,
            int result)
    : m_Name(name), m_StudentID(studentID),
    m_Test(test), m_Result(result) {
    }
    string m_Name;
    unsigned int m_StudentID;
    string m_Test;
    int m_Result;
};
#endif /* __PROGTEST__ */

typedef multimap<string, CResult> examsMultimap;

struct CStudent {
    int id;
    string name;
    map<string, int> cards;
    map<string, bool> examsRegisteredTo;
};

class CExam {
    map<int, CStudent> studentRegister;
    map<string, int> allCards;
    //examsMultimap exams;
    list<CResult> classifiedStudents;
    CStudent parseLine(string);

public:
    static const int SORT_NONE = 0;
    static const int SORT_ID = 1;
    static const int SORT_NAME = 2;
    static const int SORT_RESULT = 3;

    bool Load(istream & cardMap);
    bool Register(const string & cardID, const string & test);
    bool Assess(unsigned int studentID, const string & test, int result);
    list<CResult> ListTest(const string & test, int sortBy) const;
    set<string> ListMissing(const string & test) const;
};

CStudent CExam::parseLine(string line) {
    CStudent newStudent;
    unsigned delimiterIndex = line.find_first_of(":");
    newStudent.id = atoi(line.substr(0, delimiterIndex).c_str());
    line = line.substr(delimiterIndex + 1);

    delimiterIndex = line.find_first_of(":");
    newStudent.name = line.substr(0, delimiterIndex);
    line = line.substr(delimiterIndex + 1);
    line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());

    cout << newStudent.id << endl;
    cout << newStudent.name << endl;

    while (true) {
        delimiterIndex = line.find_first_of(",");
        string newCardId = line.substr(0, delimiterIndex);
        line = line.substr(delimiterIndex + 1);
        if (delimiterIndex == 0) continue;
        cout << "<" << newCardId << ">" << endl;
        newStudent.cards.insert(make_pair(newCardId, newStudent.id));
        if (delimiterIndex == (unsigned) string::npos) break;
    }
    cout << "----------------" << endl;
    return newStudent;
}

bool CExam::Load(istream& cardMap) {
    map<int, CStudent> newStudentRegister;
    map<string, int> newCards;
    string line;
    while (getline(cardMap, line)) {
        CStudent newStudent = parseLine(line);
        //check for duplicity student
        if (newStudentRegister.find(newStudent.id) != newStudentRegister.end() ||
                studentRegister.find(newStudent.id) != studentRegister.end()) {
            cout << "student already exists!" << endl;
            return false;
        }

        //check for duplicity card
        map<string, int>::iterator cardIt;
        for (cardIt = newStudent.cards.begin(); cardIt != newStudent.cards.end(); cardIt++) {
            if (newCards.find(cardIt->first) != newCards.end() ||
                    allCards.find(cardIt->first) != allCards.end()) {
                cout << "duplicity card" << endl;
                return false;
            }
        }

        newStudentRegister.insert(make_pair(newStudent.id, newStudent));
        newCards.insert(newStudent.cards.begin(), newStudent.cards.end());
    }

    studentRegister.insert(newStudentRegister.begin(), newStudentRegister.end());
    allCards.insert(newCards.begin(), newCards.end());

    return true;
}

bool CExam::Register(const string& cardID, const string& test) {
    if (allCards.find(cardID) == allCards.end()) {
        cout << "student not found" << endl;
        return false;
    }
    int studentId = allCards.find(cardID)->second;
    CStudent student = studentRegister.find(studentId)->second;
    if (student.examsRegisteredTo.find(test) != student.examsRegisteredTo.end()) {
        cout << "student have already registered to this exam" << endl;
        return false;
    }
    studentRegister.find(studentId)->second.examsRegisteredTo.insert(make_pair(test, false));

    return true;
}

bool CExam::Assess(unsigned int studentID, const string& test, int result) {
    if (studentRegister.count(studentID) == 0) {
        cout << "student does not exist" << endl;
        return false;
    }
    CStudent student = studentRegister.find(studentID)->second;
    if (student.examsRegisteredTo.count(test) == 0) {
        cout << "student is not registered to this test or test does not exist" << endl;
        return false;
    }
    if (student.examsRegisteredTo.find(test)->second) {
        cout << "student have already been classified" << endl;
        return false;
    }

    CResult resultRecord(student.name, student.id, test, result);
    classifiedStudents.push_back(resultRecord);
    studentRegister.find(studentID)->second.examsRegisteredTo.find(test)->second = true;
    return true;
}

list<CResult> CExam::ListTest(const string& test, int sortBy) const {
    list<CResult> returnList;
    if (sortBy == SORT_NONE) {
        for (list<CResult>::const_iterator it = classifiedStudents.begin(); it != classifiedStudents.end(); it++) {
            if (it->m_Test == test) {
                returnList.push_back(*it);
            }
        }
    } else if (sortBy == SORT_ID) {
        map<int, CResult> container;
        for (list<CResult>::const_iterator it = classifiedStudents.begin(); it != classifiedStudents.end(); it++) {
            if (it->m_Test == test) {
                container.insert(make_pair(it->m_StudentID, *it));
            }
        }
        for (map<int, CResult>::const_iterator it = container.begin(); it != container.end(); it++) {
            returnList.push_back(it->second);
        }
    } else if (sortBy == SORT_NAME) {
        map<string, CResult> container;
        for (list<CResult>::const_iterator it = classifiedStudents.begin(); it != classifiedStudents.end(); it++) {
            if (it->m_Test == test) {
                container.insert(make_pair(it->m_Name, *it));
            }
        }
        for (map<string, CResult>::const_iterator it = container.begin(); it != container.end(); it++) {
            returnList.push_back(it->second);
        }
    } else if (sortBy == SORT_RESULT) {
        map<int, CResult> container;
        for (list<CResult>::const_iterator it = classifiedStudents.begin(); it != classifiedStudents.end(); it++) {
            if (it->m_Test == test) {
                container.insert(make_pair(it->m_Result, *it));
            }
        }
        for (map<int, CResult>::const_iterator it = container.begin(); it != container.end(); it++) {
            returnList.push_front(it->second);
        }
    }
    return returnList;
}

set<string> CExam::ListMissing(const string& test) const {
    set<string> returnSet;
    for(map<int, CStudent>::const_iterator it = studentRegister.begin(); it != studentRegister.end(); it++) {
        if(it->second.examsRegisteredTo.find(test) != it->second.examsRegisteredTo.end()) {
            if(it->second.examsRegisteredTo.find(test)->second == false) {
                returnSet.insert(it->second.name);
            }
        }
    }
    return returnSet;
}

#ifndef __PROGTEST__

string toString(const list<CResult> & l) {
    ostringstream oss;
    for (auto const & x : l)
        oss << x . m_Name << ":" << x . m_StudentID << ":" << x . m_Test << ":" << x . m_Result << endl;
    return oss . str();
}

string toString(const set<string> & s) {
    ostringstream oss;
    for (const auto & x : s)
        oss << x << endl;
    return oss . str();
}

int main(void) {
    istringstream iss;
    CExam m;
    iss . clear();
    iss . str("123456:Smith John:er34252456hjsd2451451, 1234151asdfe5123416, asdjklfhq3458235\n"
            "654321:Nowak Jane: 62wtsergtsdfg34\n"
            "456789:Nowak Jane: okjer834d34\n"
            "987:West Peter Thomas:sdswertcvsgncse\n"
            "46:Lubo: a\n");

    assert(m . Load(iss));

    assert(m . Register("62wtsergtsdfg34", "PA2 - #1"));
    assert(m . Register("62wtsergtsdfg34", "PA2 - #2"));
    assert(m . Register("er34252456hjsd2451451", "PA2 - #1"));
    assert(m . Register("er34252456hjsd2451451", "PA2 - #3"));
    assert(m . Register("sdswertcvsgncse", "PA2 - #1"));
    assert(!m . Register("1234151asdfe5123416", "PA2 - #1"));
    assert(!m . Register("aaaaaaaaaaaa", "PA2 - #1"));
    assert(m . Assess(123456, "PA2 - #1", 50));
    assert(m . Assess(654321, "PA2 - #1", 30));
    assert(m . Assess(654321, "PA2 - #2", 40));
    assert(m . Assess(987, "PA2 - #1", 100));
    assert(!m . Assess(654321, "PA2 - #1", 35));
    assert(!m . Assess(654321, "PA2 - #3", 35));
    assert(!m . Assess(999999, "PA2 - #1", 35));
    assert(toString(m . ListTest("PA2 - #1", CExam::SORT_RESULT)) ==
            "West Peter Thomas:987:PA2 - #1:100\n"
            "Smith John:123456:PA2 - #1:50\n"
            "Nowak Jane:654321:PA2 - #1:30\n");
    assert(toString(m . ListTest("PA2 - #1", CExam::SORT_NAME)) ==
            "Nowak Jane:654321:PA2 - #1:30\n"
            "Smith John:123456:PA2 - #1:50\n"
            "West Peter Thomas:987:PA2 - #1:100\n");
    assert(toString(m . ListTest("PA2 - #1", CExam::SORT_NONE)) ==
            "Smith John:123456:PA2 - #1:50\n"
            "Nowak Jane:654321:PA2 - #1:30\n"
            "West Peter Thomas:987:PA2 - #1:100\n");
    assert(toString(m . ListTest("PA2 - #1", CExam::SORT_ID)) ==
            "West Peter Thomas:987:PA2 - #1:100\n"
            "Smith John:123456:PA2 - #1:50\n"
            "Nowak Jane:654321:PA2 - #1:30\n");
    assert(toString(m . ListMissing("PA2 - #3")) ==
            "Smith John\n");
    iss . clear();
    iss . str("888:Watson Joe:25234sdfgwer52, 234523uio, asdf234235we, 234234234\n");
    assert(m . Load(iss));

    assert(m . Register("234523uio", "PA2 - #1"));
    assert(m . Assess(888, "PA2 - #1", 75));
    iss . clear();
    iss . str("555:Gates Bill:ui2345234sdf\n"
            "888:Watson Joe:2345234634\n");
    assert(!m . Load(iss)); // duplicate id 888

    assert(!m . Register("ui2345234sdf", "PA2 - #1"));
    iss . clear();
    iss . str("555:Gates Bill:ui2345234sdf\n"
            "666:Watson Thomas:okjer834d34\n"); // duplicate card id okjer834d34
    assert(!m . Load(iss));

    assert(!m . Register("ui2345234sdf", "PA2 - #3"));
    iss . clear();
    iss . str("555:Gates Bill:ui2345234sdf\n"
            "666:Watson Thomas:jer834d3sdf4\n");
    assert(m . Load(iss));

    assert(m . Register("ui2345234sdf", "PA2 - #3"));
    assert(toString(m . ListMissing("PA2 - #3")) ==
            "Gates Bill\n"
            "Smith John\n");

    return 0;
}
#endif /* __PROGTEST__ */