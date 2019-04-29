public class JniTest {
    public native int c_func(int i);
    public static void main(String args[])
    {
        JniTest j = new JniTest();
        System.loadLibrary("jnitest");
        j.c_func(1);
        System.out.println("complete");
    }
}
