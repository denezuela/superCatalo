import QtQuick 2.5

Rectangle
{
  id: button
  width: 24
  height: 24
  color: "#4ea9cc"
  border.color: "#3877a8"
  radius: 10

  Image {
      width: 24;
      height: 24;
      source: "image/close.png"
  }
  MouseArea
  {
    // Действует в пределах всего элемента Image
    anchors.fill: parent
    id: mouseArea
    onClicked: callbackClose()
  }

  states:[
    State
    {
      name: "hovered";
      // Указание на то, когда элемент переходит в это состояние
      // в данном случае когда нажата левая кнопка мыши
      when: mouseArea.pressed;
      // Какие свойства будут изменяться в этом состоянии (прозрачность)
      PropertyChanges { target: button; opacity: 0.4;}
    },
    State
    {
      name: "normal"
      // В это состояние элемент будет переходить когда левая кнопка мыши не нажата
      when: mouseArea.pressed == false;
      PropertyChanges { target: button; opacity: 1; }
    }
   ]

}

