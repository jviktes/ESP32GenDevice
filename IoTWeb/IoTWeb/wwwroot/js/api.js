function callApi(endpoint, token) {

    const headers = new Headers();
    const bearer = `Bearer ${token}`;

    headers.append("Authorization", bearer);

    const options = {
        method: "GET",
        headers: headers
    };
    //UI - display token, claims etc:
    UIRenderAccessTokenData(token);
    UIRenderAccessTokenData(token,endpoint);
    UIClearAPIMessages();

    fetch(endpoint, options)
        .then(response => response.json())
        .then(response => {
            if (response) {
                response.forEach(msg => {
                    UIRenderAPIMessages(msg);
                });
            }
            return response;
        }).catch(error => {
            console.error(error);
        });
}


