import QtQuick 2.0
import QtAddOn.JsonDb 1.0


Rectangle {
    width: 360
    height: 360

    property int fontsize: 20

    JsonDb {
        id: jsondb
    }

    function errorcb(e)
    {
        console.error('Error: ' + e);
        text.text = 'Error: ' + e;
    }

    function createViewSchema(cb)
    {
        var schema = {
            "_type": "_schemaType",
            "name": "PhoneView",
            "schema": {
                "extends": "View"
            }
        };
        jsondb.query('[?_type="_schemaType"][?name="PhoneView"][/_type]', function (r) {
                         if (r.data.length > 0) {
                             cb(r.data[0])
                         } else {
                             jsondb.create(schema, cb, function (e) { console.log(e) });
                         }
                     });
    }

//! [Creating a Map Object]
    function createMapDefinition(cb)
    {
        console.log("Creating map");
        var mapDefinition = {
            "_type": "Map",
            "targetType": "PhoneView",
            "map": {
                "Contact": (function (c) {
                    for (var i in c.phoneNumbers) {
                        var info = c.phoneNumbers[i];
                        for (var k in info)
                            jsondb.emit({ "key": info[k], "firstName": c.firstName, "lastName": c.lastName});
                    }
                }
                           ).toString()
            }
        };
        return mapDefinition;
    }
//! [Creating a Map Object]

//! [Installing the Map Object]
    function installMap(cb)
    {
        mapDefinition = createMapDefinition();
        jsondb.query('[?_type="Map"][?targetType="PhoneView"]', function (r) {
                         if (r.data.length > 0) {
                             cb(r.data[0])
                         } else {
                             jsondb.emit(mapDefinition, cb, function (e) { console.log(e) });
                         }
                     });
    }
//! [Installing the Map Object]

    JsonDbListModel {
        id: contacts
        query: '[?_type="PhoneView"][/key]'
        roleNames: ["key", "value"]
        limit: 40
    }

    Rectangle {
        id: buttonAdd
        anchors.top: parent.top
        anchors.margins: 2
        width: parent.width/4
        height: 50
        color: 'gray'
        border.color: "black"
        border.width: 5
        radius: 10
        Text {
            anchors.centerIn: parent
            text: "Update"
            font.pointSize: fontsize
        }
        MouseArea {
            anchors.fill: parent
            onClicked: { createViewSchema(createMap); contacts.query = '[?_type="PhoneView"]'; }
        }
    }

    ListView {
        id: listView
        anchors.top: buttonAdd.bottom
        anchors.bottom: statusText.top
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        width: parent.width
        model: contacts
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 ;width: 200;}
        focus: true
        delegate: Row {
            spacing: 10
            Text {
                text: key + ":   " + value.firstName + ", " + value.lastName
                font.pointSize: fontsize
                MouseArea {
                   anchors.fill: parent;
                   onPressed: {
                       listView.currentIndex = index;
                   }
                }
            }
        }
    }
    Rectangle {
        id: statusText
        anchors.bottom: messageRectangle.top
        width: parent.width
        height: 20
        color:  "lightgray"
        Text {
            anchors.centerIn: parent
            font.pointSize: fontsize
            text: "limit : " + contacts.limit + "  rowCount : " + contacts.rowCount + "  state : " + contacts.state
        }
    }
    Rectangle {
        id: messageRectangle
        anchors.bottom: parent.bottom
        width: parent.width
        height: 24
        color: "white"
        Text {
            id: messageText
            anchors.centerIn: parent
            font.pointSize: fontsize
            color: "red"
            text: ""
        }
    }
}
