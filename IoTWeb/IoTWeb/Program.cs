using Microsoft.Extensions.Options;

var MyAllowSpecificOrigins = "_myAllowSpecificOrigins";

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
builder.Services.AddRazorPages();
//allow cors (SPA use api):
//builder.Services.AddCors(opt =>
//{
//    //opt.AddDefaultPolicy(builder =>
//    //{
//    //    builder.AllowAnyOrigin()
//    //        .AllowAnyHeader()
//    //        .AllowAnyMethod();
//    //});
//    opt.AddPolicy(name: MyAllowSpecificOrigins,
//                      policy =>
//                      {
//                          policy.WithOrigins("https://getiotdatavik.azurewebsites.net",
//                                              "http://www.contoso.com");
//                      });
//});

var app = builder.Build();

// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Error");
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}

app.UseHttpsRedirection();
app.UseStaticFiles();

app.UseRouting();
//app.UseCors();
app.UseAuthorization();

app.MapRazorPages();

app.Run();
