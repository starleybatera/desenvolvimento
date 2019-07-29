// Inicia o firebase Firebase
var config = {
  apiKey: "AIzaSyDoFUDsBBndpzLjcXI4i6eGbc3X2vymVRY",
  authDomain: "sistema-de-irrigacao.firebaseapp.com",
  databaseURL: "https://sistema-de-irrigacao.firebaseio.com",
  projectId: "sistema-de-irrigacao",
  storageBucket: "sistema-de-irrigacao.appspot.com",
  messagingSenderId: "1017790599393",
  appId: "1:1017790599393:web:b516fffbff3580f1"
};
firebase.initializeApp(config);

var db = firebase.database();



(function () {

  
  // Cria os listeners dos dados no firebase
  // var chuvaRef = db.ref('sensor_chuva');
  // var tempRef = db.ref('sensor_T');
  // var umidRef = db.ref('sensor_H');
  // var sensor1Ref = db.ref('sensor1_H');
  // var tensao1Ref = db.ref('sensor1_T');
  // var sensor2Ref = db.ref('sensor2_H');
  // var tensao2Ref = db.ref('sensor2_T');
  // var piquete_irrigandoRef = db.ref('piquete_irrigando');
  // var bombRef = db.ref('bomba');
  var chuvaRef = db.ref('servidor/sensor_chuva');
  var tempRef = db.ref('servidor/sensor_T');
  var umidRef = db.ref('servidor/sensor_H');
  var sensor1Ref = db.ref('sensor1/sensor1_H');
  var tensao1Ref = db.ref('sensor1/sensor1_T');
  var sensor2Ref = db.ref('sensor2/sensor2_H');
  var tensao2Ref = db.ref('sensor2/sensor2_T');
  var piquete_irrigandoRef = db.ref('servidor/piquete_irrigando');
  var bombRef = db.ref('servidor/bomba');
  



  var chuvaRef = db.ref('servidor/sensor_chuva');
  var tempRef = db.ref('servidor/sensor_T');
  var umidRef = db.ref('servidor/sensor_H');
  var piquete_irrigandoRef = db.ref('servidor/piquete_irrigando');
  var bombRef = db.ref('servidor/bomba');
  var sensor1Ref = db.ref('sensor1/sensor1_H');
  var sensor2Ref = db.ref('sensor2/sensor2_H');





  tempRef.on('value', dadosCorrente('currentTemp', 'tempLineChart', 'Temperatura', 'C°'));
  umidRef.on('value', dadosCorrente('currentUmid', 'umidLineChart', 'Umidade', '%'));
  sensor1Ref.on('value', dadosCorrente('currentUmidSensor1', 'umidSensor1LineChart', 'Umidade', '%'));
  sensor2Ref.on('value', dadosCorrente('currentUmidSensor2', 'umidSensor2LineChart', 'Umidade', '%'));
  //chuvaRef.on('value', snap => currentChuva.innerText = snap.val());
  //tempRef.on('value', snap => currentTemp.innerText = snap.val());
  //umidRef.on('value', snap => currentUmid.innerText = snap.val());
  //sensor1Ref.on('value', snap => currentUmidSensor1.innerText = snap.val());
  //intensao1Ref.on('value', snap => currentTensoSensor1.innerText = snap.val());
  //sensor2Ref.on('value', snap => currentUmidSensor2.innerText = snap.val());
  //tensao2Ref.on('value', snap => currentTensoSensor2.innerText = snap.val());


  // // Registrar função ao alterar valor do Piquete q está sendo irrigado

  piquete_irrigandoRef.on('value', function (snapshot) {
    
    var value = snapshot.val();
    console.log("VALUE", value);
    var aux = value;
    var piquete = document.getElementById('currentIrrigando')
    // if(aux != piquete.innerText){
    //   zera();
    // }
    piquete.innerText = value
    console.log("piquete.innerText", piquete.innerText);
    // inicia();
  });

  // // Registrar função ao alterar valor do status do clima
  chuvaRef.on('value', function (snapshot) {
    var value = snapshot.val();
    var chuva = document.getElementById('currentChuva')
    //console.log("Nivel sensor chuva: ", value);
    if (value > 800) {
      chuva.innerText = "wb_sunny"
    } else {
      chuva.innerText = "wb_cloudy"
    }

  });

  // // Registrar função ao alterar valor da bomba dagua
  var currentBombValue = false;
  bombRef.on('value', function (snapshot) {
    var value = snapshot.val();
    var bomb = document.getElementById('currentBomb')
    if (value) {
      bomb.classList.add('amber-text');
      // inicia();
    } else {
      bomb.classList.remove('amber-text');
    }
    currentBombValue = !!value;
  });


  // Registrar função de click no botão de ligar e desligar a bomba
  var btnbomb = document.getElementById('btn-bomb');
  btnbomb.addEventListener('click', function (evt) {
    bombRef.set(!currentBombValue);
    console.log("STATUS BOTAO", currentBombValue);
    if (currentBombValue == true) {
      btnbomb.innerText = "Desligar";
      btnbomb.style.backgroundColor = "#000";
    }
    else {
      btnbomb.innerText = "Ligar";
      btnbomb.style.backgroundColor = "#FFC107";
    }
  });

})();

function dadosCorrente(id_corrente, id_grafico, texto, metrica) {
  return function (snapshot) {
    var valor = snapshot.val();
    console.log("ID_corrente: ", id_corrente)
    console.log("Valor: ", valor)
    //if(valor){
    //console.log("Valor-TRUE" ,valor)
    // var valorCorrente;
    var dados = [];
    // for(var key in valor){
    //   console.log("Valor[key]" ,key)
    //   valorCorrente = valor[key]
    //   dados.push(valorCorrente);
    // }

    document.getElementById(id_corrente).innerText = valor + ' ' + metrica;
    criarGrafico(id_grafico, texto, dados);
    //}

  }
}

function criarGrafico(id_grafico, texto, dados) {
  var no = document.getElementById(id_grafico);
  new Chart(no, {
    type: 'line',
    dados: {
      labels: new Array(dados.length).fill(""),
      datasets: [{
        texto: texto,
        dados: dados,
        borderWidth: 1,
        fill: false,
        spanGaps: false,
        lineTension: 0.1,
        backgroundColor: "#F9A825",
        borderColor: "#F9A825"
      }]
    }
  });
}



// function formatatempo(segs) {
//   min = 0;
//   hr = 0;

//   while (segs >= 60) {
//     if (segs >= 60) {
//       segs = segs - 60;
//       min = min + 1;
//     }
//   }

//   while (min >= 60) {
//     if (min >= 60) {
//       min = min - 60;
//       hr = hr + 1;
//     }
//   }

//   if (hr < 10) { hr = "0" + hr }
//   if (min < 10) { min = "0" + min }
//   if (segs < 10) { segs = "0" + segs }

//   fin = `<b>${hr}</b>h <b>${min}</b>m <b>${segs}</b>s`;
//   db.ref('servidor/cronometro').set(fin);
//   return fin;

// }
// var segundos = 0; //inicio do cronometro
// var interval;

// function conta() {
//   segundos++;
//   document.getElementById("tempo").innerHTML = formatatempo(segundos);
// }

// function inicia() {
//   interval = setInterval("conta();", 1000);
// }

// function para() {
//   clearInterval(interval);
// }

// function zera() {
//   clearInterval(interval);
//   segundos = 0;
//   document.getElementById("tempo").innerHTML = formatatempo(segundos);
// }



