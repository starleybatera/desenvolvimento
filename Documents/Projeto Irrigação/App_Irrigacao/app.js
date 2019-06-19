
(function(){
    
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

  // Cria os listeners dos dados no firebase
  var chuvaRef = db.ref('sensor_chuva');
  var tempRef = db.ref('sensor_T');
  var umidRef = db.ref('sensor_H');
  var sensor1Ref = db.ref('sensor1_H');
  var tensao1Ref = db.ref('sensor1_T');
  var sensor2Ref = db.ref('sensor2_H');
  var tensao2Ref = db.ref('sensor2_T');
  var piquete_irrigandoRef = db.ref('piquete_irrigando');
  //var presenceRef = db.ref('presence');
  var bombRef = db.ref('bomba');


  // Registra as funções que atualizam os gráficos e dados atuais da telemetria
  //tempRef.on('value', onNewData('currentTemp', 'tempLineChart' , 'Temperatura', 'C°'));
  //chuvaRef.on('value', snap => currentChuva.innerText = snap.val());
  tempRef.on('value', snap => currentTemp.innerText = snap.val());
  umidRef.on('value', snap => currentUmid.innerText = snap.val());
  sensor1Ref.on('value', snap => currentUmidSensor1.innerText = snap.val());
  tensao1Ref.on('value', snap => currentTensoSensor1.innerText = snap.val());
  sensor2Ref.on('value', snap => currentUmidSensor2.innerText = snap.val());
  tensao2Ref.on('value', snap => currentTensoSensor2.innerText = snap.val());
  
  //umidRef.on('value', graficoCorrente('currentUmid', 'umidLineChart' , 'Umidade', '%'));


  // Registrar função ao alterar valor de presença
  // presenceRef.on('value', function(snapshot){
  //   var value = snapshot.val();
  //   var el = document.getElementById('currentPresence')
  //   if(value){
  //     el.classList.add('green-text');
  //   }else{
  //     el.classList.remove('green-text');
  //   }
  // });

  // // Registrar função ao alterar valor do Piquete q está sendo irrigado
  
  piquete_irrigandoRef.on('value', function(snapshot){
    var value = snapshot.val();
    var chuva = document.getElementById('currentIrrigando')
      chuva.innerText =  value
    
  });

  chuvaRef.on('value', function(snapshot){
    var value = snapshot.val();
    var chuva = document.getElementById('currentChuva')
    console.log("Nivel sensor chuva: ", value);
    if(value == 1024){
      chuva.innerText = "TEMPO NORMAL"
    }else{
      chuva.innerText =  "TEMPO CHUVOSO !"
    }
    
  });

  // // Registrar função ao alterar valor da bomba dagua
  var currentBombValue = false;
  bombRef.on('value', function(snapshot){
    var value = snapshot.val();
    var bomb = document.getElementById('currentBomb')
    if(value){
      bomb.classList.add('amber-text');
    }else{
      bomb.classList.remove('amber-text');
    }
    currentBombValue = !!value;
  });


  // Registrar função de click no botão de ligar e desligar a bomba
  var btnbomb = document.getElementById('btn-bomb');
  btnbomb.addEventListener('click', function(evt){
    bombRef.set(!currentBombValue);
  });

})();


// Retorna uma função que de acordo com as mudanças dos dados
// Atualiza o valor atual do elemento, com a metrica passada (valorCorrenteEl e metrica)
// e monta o gráfico com os dados e descrição do tipo de dados (chartEl, label)
function graficoCorrente(valorCorrenteEl, chartEl, label, metrica){
  return function(snapshot){
    var lendo = snapshot.val();
    if(lendo){
        var valorCorrente;
        var data = [];
        for(var key in lendo){
          valorCorrente = lendo[key]
          data.push(valorCorrente);
        }

        document.getElementById(valorCorrenteEl).innerText = valorCorrente + ' ' + metrica;
        buildLineChart(chartEl, label, data);
    }
  }
}

// Constroi um gráfico de linha no elemento (el) com a descrição (label) e os
// dados passados (data)
function buildLineChart(el, label, data){
  var elNode = document.getElementById(el);
  new Chart(elNode, {
    type: 'line',
    data: {
        labels: new Array(data.length).fill(""),
        datasets: [{
            label: label,
            data: data,
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
