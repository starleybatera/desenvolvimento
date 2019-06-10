(function() {
  const config = {
      apiKey: "AIzaSyDoFUDsBBndpzLjcXI4i6eGbc3X2vymVRY",
      authDomain: "sistema-de-irrigacao.firebaseapp.com",
      databaseURL: "https://sistema-de-irrigacao.firebaseio.com",
      projectId: "sistema-de-irrigacao",
      storageBucket: "sistema-de-irrigacao.appspot.com",
      messagingSenderId: "1017790599393",
      appId: "1:1017790599393:web:b516fffbff3580f1"
  };

  firebase.initializeApp(config);

  
  const sensor_H = document.getElementById('sensor_H');
  const dbsensor_HRef = firebase.database().ref().child('sensor_H');
  dbsensor_HRef.on('value', snap => sensor_H.innerText = snap.val());

  const sensor_T = document.getElementById('sensor_T');
  const dbsensor_TRef = firebase.database().ref().child('sensor_T');
  dbsensor_TRef.on('value', snap => sensor_T.innerText = snap.val() );


}());
