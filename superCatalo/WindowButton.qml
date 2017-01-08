import QtQuick 2.5

Image
{
  id: button
  width: 25
  height: 25
  MouseArea
  {
    // Действует в пределах всего элемента Image
    anchors.fill: parent
    id: mouseArea
    onClicked: callback()
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
//  Behavior on opacity
//  {
//    // Анимация с шагом в 100 миллисекунд
//    // Раз в 100 миллисекунд прозрачность будет изменяться на 0,1
//    NumberAnimation { duration: 100 }
//  }
}

