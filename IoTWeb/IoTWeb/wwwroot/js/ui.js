// Select DOM elements to work with
const signInButton = document.getElementById('signIn');
const signOutButton = document.getElementById('signOut')
const welcomeDiv = document.getElementById('welcome-div');

const signInInfoDiv = document.getElementById('id-sign-in-info');

const contentIDToken = document.getElementById('div-id-token');
const tableAccessTokenClaims = document.getElementById('div-access-token');
const APIresponse = document.getElementById("id-API-response");
const apiContent = document.getElementById('apiContent');

function welcomeUser(username) {
    //label.classList.add('d-none');
    signInButton.classList.add('d-none');
    //signInInfoDiv.classList.add('d-none');
    //signOutButton.classList.remove('d-none');
    welcomeDiv.classList.remove('d-none');
    welcomeDiv.innerHTML = `Welcome ${username}!`
    //callApiButton.classList.remove('d-none');
    //apiContent.classList.remove('d-none');
}

function UIClearAPIMessages() {
    APIresponse.innerHTML = '';
}

function UIRenderAPIMessages(s) {
    $("#"+APIresponse.id).append('<pre>' + JSON.stringify(s) + '</pre>');
}

function UIRenderIDTokenData(response) {

    contentIDToken.innerHTML = '';

    if (response==null || response.idTokenClaims==null || response.idTokenClaims==undefined) {
        return;
    }

    contentIDToken.innerHTML = '';
    var token_parts = response.idToken.split('.');

    $("#"+contentIDToken.id).append('<h5>Token info:</h5>');
    $("#"+contentIDToken.id).append('<h6>Expires:</h6>');
    $("#"+contentIDToken.id).append('<h6>'+response.expiresOn+'</h6>');
    $("#"+contentIDToken.id).append('<h6>ID token header:</h6>');
    $("#"+contentIDToken.id).append('<pre>' + JSON.stringify(JSON.parse(atob(token_parts[0])), null, 2) + '</pre>');
    $("#"+contentIDToken.id).append('<h6>ID token payload:</h6>');
    $("#"+contentIDToken.id).append('<pre>' + JSON.stringify(JSON.parse(atob(token_parts[1])), null, 2) + '</pre>');
    $("#"+contentIDToken.id).append('<h6>Raw token:</h6>');
    $("#"+contentIDToken.id).append('<p>'+response.idToken+'</p>');

}

function UIRenderAccessTokenData(access_token) {

    tableAccessTokenClaims.innerHTML = '';

    if (access_token==null) {
        return;
    }

    var token_parts = access_token.split('.');

    $("#"+tableAccessTokenClaims.id).append('<h5>Token info:</h5>');
    $("#"+tableAccessTokenClaims.id).append('<h6>Expires:</h6>');
    $("#"+tableAccessTokenClaims.id).append('<h6>'+new Date(JSON.parse(atob(token_parts[1])).exp*1000).toString()+'</h6>');
    $("#"+tableAccessTokenClaims.id).append('<h6>Access token header:</h6>');
    $("#"+tableAccessTokenClaims.id).append('<pre>' + JSON.stringify(JSON.parse(atob(token_parts[0])), null, 2) + '</pre>');
    $("#"+tableAccessTokenClaims.id).append('<h6>Access token payload:</h6>');
    $("#"+tableAccessTokenClaims.id).append('<pre>' + JSON.stringify(JSON.parse(atob(token_parts[1])), null, 2) + '</pre>');
    $("#"+tableAccessTokenClaims.id).append('<h6>Raw token:</h6>');
    $("#"+tableAccessTokenClaims.id).append('<p>'+access_token+'</p>');

}

function setActiveItem(_item) {

    var _menu=["tokens","api","bad_practice"];

    //menu:
    _menu.forEach(element => {
        var id="id_menu_"+element;
        var x=document.getElementById(id);
        x.classList.remove("active");
    });

    //content:
    _menu.forEach(element => {
        var id="id_"+element+"_content";
        var x = document.getElementById(id);
        x.style.display = "none";
    });

    document.getElementById("id_"+_item+"_content").style.display="block";
    document.getElementById("id_menu_"+_item).classList.add("active");

    passTokenToApi();

}

function parseJwt (token) {
    return JSON.parse(Buffer.from(token.split('.')[1], 'base64').toString());
}
//bad practise - no input validation
function processInputURLImg() 
{
    var userPickedImageUrl = $("#id-input-dataA").val();
    const contentWithUserInput = `<img src="${userPickedImageUrl}">`;
    var ele = "id-resultsA";
    document.getElementById(ele).innerHTML = contentWithUserInput;
}
//bad practise - no input validation
function processGeneralInput() 
{
    var userPickedImageUrl = $("#id-input-dataB").val();
    const contentWithUserInput = userPickedImageUrl;
    var ele = "id-resultsB";
    document.getElementById(ele).innerHTML = contentWithUserInput;
}
