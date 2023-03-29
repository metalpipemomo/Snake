#import "Bridge.h"

#import "GameObject.h"


@interface Bridge() {
    GLKView *glkView;
    std::chrono::time_point<std::chrono::steady_clock> lastTime;
    GameObject *test;
}

@end

@implementation Bridge

@synthesize Moving;
@synthesize Rotating;
@synthesize CameraZoom;
@synthesize CameraRotation;

- (void)dealloc
{
    glDeleteProgram(test->GetComponent<MeshRenderer>()->renderer->program);
}

- (void)loadModels
{
    
}

- (void)setup:(GLKView *)view
{
    view.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (!view.context) {
        NSLog(@"Failed to create ES Context");
    }
    
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    glkView = view;
    [EAGLContext setCurrentContext:view.context];
    
    Camera::current = new Camera({ 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, -5.0f }, (60.0f * M_PI) / 180.0f, 1.0f, 1.0f, 70.0f);
    test = new GameObject();
    test->AddComponent(new Transform());
    const char *vname = "TrueBasic.vsh";
    const char *fname = "TrueBasic.fsh";
    const char *objname = "Cube.obj";
    NSString *texName = @"crate.jpg";
    Renderer *r = new Renderer(vname, fname, texName);
    
    Mesh *m = new Mesh(objname);
    MeshRenderer *mr = new MeshRenderer();
    mr->SetMesh(m);
    mr->SetRenderer(r);
    test->AddComponent(mr);
    
    Moving = false;
    Rotating = false;
    CameraZoom = 1.0f;
    CameraRotation = CGPointMake(0, 0);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    lastTime = std::chrono::steady_clock::now();
}

- (void)update
{
    // taking time
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
    lastTime = currentTime;
    
    //projection matrix
    float aspectRatio = (float) glkView.drawableWidth / (float) glkView.drawableHeight;
    Camera::current->SetAspectRatio(aspectRatio);
    test->UpdateComponents();
    
}

- (void)draw:(CGRect)drawRect
{
    glViewport(0, 0, (int) glkView.drawableWidth, (int) glkView.drawableHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw vertices here
    auto mr = test->GetComponent<MeshRenderer>();
    mr->Draw();
}

@end
