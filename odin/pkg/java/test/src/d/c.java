package d;

public class c {
    public static int f1() {
        return 2;
    }
    public static int f() {
        return 7+e.f();
    }
    private class p {
        public int f() {
            return 9;
        }
    }
    public static int g() {
        return new c().gg();
    }
    private int gg() {
        return new p().f()+1;
    }
}
