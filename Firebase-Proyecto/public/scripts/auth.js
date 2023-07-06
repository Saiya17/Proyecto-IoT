// escucha los cambios de estado de autenticación
auth.onAuthStateChanged(user => {
    if (user) {
      console.log("Usuario conectado");
      console.log(user);
      setupUI(user);
      var uid = user.uid;
      console.log(uid);
    } else {
      console.log("Usuario desconectado");
      setupUI();
    }
   });
   
 // acceso
   const loginForm = document.querySelector('#login-form');
   loginForm.addEventListener('submit', (e) => {
    e.preventDefault();
   // obtener información del usuario
    const email = loginForm['input-email'].value;
    const password = loginForm['input-password'].value;
    // inicia sesión en la usuario
    auth.signInWithEmailAndPassword(email, password).then((cred) => {
      // cerrar el modal de inicio de sesión y restablecer el formulario
      loginForm.reset();
      console.log(email);
    })
    .catch((error) =>{
      const errorCode = error.code;
      const errorMessage = error.message;
      document.getElementById("error-message").innerHTML = errorMessage;
      console.log(errorMessage);
    });
   });
   
  // cerrar sesión
   const logout = document.querySelector('#logout-link');
   logout.addEventListener('click', (e) => {
    e.preventDefault();
    auth.signOut();
   });
   