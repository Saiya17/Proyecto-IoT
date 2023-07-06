const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");

// Elementos para lecturas de sensores
const tempElement = document.getElementById("temp");
const humElement = document.getElementById("hum");
const presElement = document.getElementById("pres");

// GESTIONAR LA IU DE INICIO/CIERRE DE SESIÓN
const setupUI = (user) => {
  if (user) {
    //alternar elementos de la interfaz de usuario
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display ='block';
    userDetailsElement.style.display ='block';
    userDetailsElement.innerHTML = user.email;

    // obtener el UID del usuario para obtener datos de la base de datos
    var uid = user.uid;
    console.log(uid);

  // Rutas de la base de datos (con UID de usuario)
    var dbPathTemp = 'UsersData/' + uid.toString() + '/TEMPERATURA';
    var dbPathHum = 'UsersData/' + uid.toString() + '/HUMEDAD';
    var dbPathPres = 'UsersData/' + uid.toString() + '/PRESION';

// referencias de bases de datos
    var dbRefTemp = firebase.database().ref().child(dbPathTemp);
    var dbRefHum = firebase.database().ref().child(dbPathHum);
    var dbRefPres = firebase.database().ref().child(dbPathPres);



// Actualizar página con nuevas lecturas
dbRefTemp.on('value', snap => {
  const tempValue = snap.val().toFixed(2);
  tempElement.innerText = tempValue;
});

dbRefHum.on('value', snap => {
  const humValue = snap.val().toFixed(2);
  humElement.innerText = humValue;
});

dbRefPres.on('value', snap => {
  const presValue = snap.val().toFixed(2);
  presElement.innerText = presValue;
});

  // Si la usuario está desconectada
  } else{
    // alternar elementos de la interfaz de usuario
    loginElement.style.display = 'block';
    authBarElement.style.display ='none';
    userDetailsElement.style.display ='none';
    contentElement.style.display = 'none';
  }
  
}